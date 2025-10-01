// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>
#include "mathutil/umath.h"
#include <functional>
#include <string>

module pragma.server_manager;

import :manager;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif

nwm::Server::Server(const std::shared_ptr<SVNWMUDPConnection> &udp, const std::shared_ptr<SVNWMTCPConnection> &tcp) : ManagerBase(udp, tcp), m_maxClients(-1)
{
	auto packetHandle = std::bind(
	  [](Server *sv, const NWMEndpoint &ep, NWMIOBase *io, uint32_t id, NetPacket &readPacket) {
		  auto *session = dynamic_cast<NWMSession *>(io);
		  //if(session == nullptr)
		  //	return;

		  auto bValidSession = (session != nullptr) ? true : false;
		  auto epSession = (bValidSession == true) ? session->GetRemoteEndPoint() : ep;
		  epSession = epSession.Copy();
		  if(id == umath::to_integral(MessageIn::Ping)) {
			  if(session != nullptr) {
				  // Special packet, we need to send the response immediately, otherwise we
				  // can't calculate the correct latency
				  auto latency = readPacket->Read<uint16_t>();
				  sv->SchedulePollEvent([sv, latency, bValidSession, epSession]() -> PollEventResult {
					  auto *svCl = (bValidSession == true) ? sv->FindClient(epSession) : nullptr;
					  if(svCl != nullptr)
						  svCl->SetLatency(latency);
					  return PollEventResult::Complete;
				  });
				  NetPacket p(umath::to_integral(MessageOut::Pong));
				  session->SendPacket(p);
			  }
			  return;
		  }

		  if(sv->HandleAsyncPacket(epSession, session, id, readPacket) == true)
			  return;

		  sv->SchedulePollEvent([epSession, bValidSession, sv, id, readPacket]() mutable -> PollEventResult {
			  auto *svCl = (bValidSession == true) ? sv->FindClient(epSession) : nullptr;
			  if(sv->HandlePacket(epSession, svCl, id, readPacket) == false)
				  sv->HandleError(Error(umath::to_integral(Result::UnhandledPacket), "Unhandled packet '" + std::to_string(id) + "'"));
			  return PollEventResult::Complete;
		  });
	  },
	  this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	auto clientHandle = [](Server *sv, NWMSession *session) {
#ifdef NWM_VERBOSE
		std::cout << "[NWMSV] Handling client..." << std::endl;
#endif
		auto address = session->GetAddress();
		auto port = session->GetPort();
		auto bUdp = session->IsUDP();
		auto bTcp = session->IsTCP();
		auto hSession = std::make_shared<NWMSessionHandle>(session->GetHandle());
		sv->SchedulePollEvent([sv, address, port, bUdp, bTcp, hSession]() -> PollEventResult {
			auto clients = sv->GetAllClients(false); // Client mutex is already locked at this point, don't lock it again!
			auto protocol = bUdp ? nwm::Protocol::UDP : nwm::Protocol::TCP;
			ServerClient *cl = nullptr;
			for(auto &client : clients) {
				if(client.IsValid() && !client->IsClosed() && !client->IsClosing() && client->UsesAddress(address)) {
					if(client->UsesPort(port)) {
#ifdef NWM_VERBOSE
						std::cout << "[NWMSV] Client already connected! Dropping previous..." << std::endl;
#endif
						sv->DropClient(client.get(), ClientDropped::Timeout);
						return PollEventResult::Complete;
					}
					else {
						if((!client->IsUDPConnected() && bUdp) || (!client->IsTCPConnected() && bTcp)) {
#ifdef NWM_VERBOSE
							std::cout << "[NWMSV] Client is fully connected!" << std::endl;
#endif
							cl = client.get();
							auto hCl = cl->GetHandle();
							sv->ScheduleAsyncEvent([sv, hCl, bTcp, hSession]() {
								if(hSession->IsValid() == false)
									return;
								sv->m_clientMutex.lock();
								if(hCl.IsValid()) {
									auto *cl = hCl.get();
									if(bTcp == true)
										cl->SetSession(static_cast<NWMTCPSession *>(hSession->get()));
									else
										cl->SetSession(static_cast<NWMUDPSession *>(hSession->get()));
									sv->UpdateClientReadyState(cl);
								}
								sv->m_clientMutex.unlock();
							});
							break;
						}
					}
				}
			}
			if(cl == nullptr) {
				sv->m_clientsConnecting.push_back(sv->CreateClient());
				cl = sv->m_clientsConnecting.back().get();
				auto hCl = cl->GetHandle();
				sv->ScheduleAsyncEvent([sv, hCl, hSession, bTcp]() {
					if(hSession->IsValid() == false)
						return;
					sv->m_clientMutex.lock();
					if(hCl.IsValid()) {
						auto *cl = hCl.get();
						if(bTcp == true)
							cl->SetSession(static_cast<NWMTCPSession *>(hSession->get()));
						else
							cl->SetSession(static_cast<NWMUDPSession *>(hSession->get()));
						sv->UpdateClientReadyState(cl);
					}
					sv->m_clientMutex.unlock();
				});
				cl->SetIndex(sv->m_clientsConnecting.size() - 1);
			}
			cl->SetAddress(protocol, address, port);
			return PollEventResult::Complete;
		});
	};
	SVNWMConnection::AcceptCallbacks acceptCallbacks {};
	acceptCallbacks.lock = [this]() { m_clientMutex.lock(); };
	acceptCallbacks.unlock = [this]() { m_clientMutex.unlock(); };
	acceptCallbacks.has_client = [this](nwm::IPAddress address, uint16_t port) -> bool { return GetClient(address, port).IsValid() ? true : false; };
	acceptCallbacks.accept_client = [this](NWMSession *session) -> bool { return AcceptClient(session); };
	if(udp != nullptr) {
		//udp->SetServer(this);
		udp->SetAcceptCallbacks(acceptCallbacks);
		udp->SetPacketHandle(packetHandle);
		udp->SetClientHandle(std::bind(clientHandle, this, std::placeholders::_1));
	}
	if(tcp != nullptr) {
		//tcp->SetServer(this);
		tcp->SetAcceptCallbacks(acceptCallbacks);
		tcp->SetPacketHandle(packetHandle);
		tcp->SetClientHandle(std::bind(clientHandle, this, std::placeholders::_1));
	}
	InitializeConnectionData();
}
nwm::Server::~Server() { Close(); }
void nwm::Server::Shutdown()
{
#ifdef NWM_VERBOSE
	std::cout << "[NWMSV] Shutting down..." << std::endl;
#endif
	for(auto &cl : m_clientsConnecting)
		DropClient(cl.get(), ClientDropped::Shutdown);
	for(auto &cl : m_clients)
		DropClient(cl.get(), ClientDropped::Shutdown);
	Close();
}
bool nwm::Server::HandlePacket(ServerClient *cl, uint32_t id, NetPacket &packet) { return false; }
bool nwm::Server::HandlePacket(const NWMEndpoint &ep, ServerClient *cl, uint32_t id, NetPacket &packet)
{
#ifdef NWM_VERBOSE
	std::cout << "[NWMSV] Handling incoming packet " << id << "..." << std::endl;
#endif
	OnPacketReceived(ep, cl, id, packet);
	if(cl == nullptr || !cl->IsFullyConnected())
		return true;
	cl->ResetLastUpdate();
	if(id == umath::to_integral(MessageIn::ClientDisconnect)) {
		DropClient(cl, ClientDropped::Disconnected, true);
		return true;
	}
	return HandlePacket(cl, id, packet);
}
bool nwm::Server::HandleAsyncPacket(const NWMEndpoint &ep, NWMSession *session, uint32_t id, NetPacket &packet) { return false; }
void nwm::Server::OnPacketReceived(const NWMEndpoint &ep, ServerClient *cl, uint32_t id, NetPacket &packet) {}
nwm::ServerClientHandle nwm::Server::GetClient(const std::string &ip, uint16_t port)
{
	auto address = boost::asio::ip::make_address(ip);
	return GetClient(address, port);
}
nwm::ServerClientHandle nwm::Server::GetClient(const nwm::IPAddress &address, uint16_t port)
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [&address, port](const std::shared_ptr<nwm::ServerClient> &client) { return client->IsTarget(address, port); });
	return (it != m_clients.end()) ? (*it)->GetHandle() : ServerClientHandle {};
}
int32_t nwm::Server::GetMaxClients() const { return m_maxClients; }
int32_t nwm::Server::GetClientCount() const { return static_cast<int32_t>(m_clients.size()); }
std::vector<nwm::ServerClientHandle> nwm::Server::GetClients() const
{
	std::vector<ServerClientHandle> clientHandles;
	GetClients(clientHandles);
	return clientHandles;
}
void nwm::Server::GetClients(std::vector<ServerClientHandle> &clients) const
{
	m_clientMutex.lock();
	clients.reserve(m_clients.size());
	for(auto &cl : m_clients)
		clients.push_back(cl->GetHandle());
	m_clientMutex.unlock();
}
void nwm::Server::SetMaxClients(int32_t numClients) { m_maxClients = numClients; }
bool nwm::Server::IsIPBanned(const std::string &ip)
{
	auto address = boost::asio::ip::make_address(ip);
	return IsIPBanned(address);
}

bool nwm::Server::IsIPBanned(const nwm::IPAddress &ip)
{
	std::lock_guard<std::mutex> lg(m_banListMutex);
	auto it = std::find(m_banned.begin(), m_banned.end(), ip);
	return (it != m_banned.end()) ? true : false;
}

void nwm::Server::BanIP(const std::string &ip)
{
	std::lock_guard<std::mutex> lg(m_banListMutex);
	m_banned.push_back(boost::asio::ip::make_address(ip)); // TODO Kick, if already connected
}
void nwm::Server::BanIPs(const std::vector<std::string> &ips)
{
	std::lock_guard<std::mutex> lg(m_banListMutex);
	m_banned.reserve(m_banned.size() + ips.size());
	for(auto &ip : ips) {
		auto address = boost::asio::ip::make_address(ip);
		if(IsIPBanned(address) == true)
			continue;
		m_banned.push_back(address);
	}
}
void nwm::Server::RemoveClient(std::vector<std::shared_ptr<ServerClient>> &clients, ServerClient *cl)
{
	auto idx = cl->GetIndex();
	clients.erase(clients.begin() + idx);
	if(idx != (clients.size() + 1)) {
		for(auto i = idx; i < clients.size(); i++)
			clients.at(i)->SetIndex(i);
	}
}
std::vector<nwm::ServerClient *> nwm::Server::GetFilterTargets(const RecipientFilter &filter)
{
	std::vector<ServerClient *> clients;
	auto &sessions = const_cast<RecipientFilter &>(filter).get();
	clients.reserve(sessions.size());
	if(filter.GetFilterType() == RecipientFilter::Type::Include) {
		for(auto &hSession : sessions) {
			if(hSession.IsValid() == false)
				continue;
			clients.push_back(hSession.get());
		}
		return clients;
	}
	for(auto &cl : m_clients) {
		auto *pCl = cl.get();
		auto it = std::find_if(sessions.begin(), sessions.end(), [pCl](const ServerClientHandle &clOther) { return (clOther.IsValid() && clOther.get() == pCl) ? true : false; });
		if(it != sessions.end())
			continue;
		sessions.push_back(pCl);
	}
	return clients;
}

std::vector<nwm::ServerClientHandle> nwm::Server::GetAllClients() const { return GetAllClients(true); }
std::vector<nwm::ServerClientHandle> nwm::Server::GetAllClients(bool bLockClients) const
{
	std::vector<ServerClientHandle> clientHandles;
	GetAllClients(clientHandles, bLockClients);
	return clientHandles;
}
void nwm::Server::GetAllClients(std::vector<ServerClientHandle> &clients) const { GetAllClients(clients, true); }
void nwm::Server::GetAllClients(std::vector<ServerClientHandle> &clients, bool bLockClients) const
{
	if(bLockClients == true)
		m_clientMutex.lock();

	clients.reserve(m_clients.size() + m_clientsConnecting.size());
	for(auto &cl : m_clients)
		clients.push_back(cl->GetHandle());
	for(auto &cl : m_clientsConnecting)
		clients.push_back(cl->GetHandle());

	if(bLockClients == true)
		m_clientMutex.unlock();
}
nwm::ServerClient *nwm::Server::FindClient(const NWMEndpoint &ep)
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [&ep](const std::shared_ptr<nwm::ServerClient> &cl) { return cl->IsTarget(ep); });
	return (it != m_clients.end()) ? it->get() : nullptr;
}
void nwm::Server::BroadcastPacket(Protocol protocol, const NetPacket &packet)
{
#ifdef NWM_VERBOSE
	std::cout << "[NWMSV] Broadcasting packet..." << std::endl;
#endif
	ScheduleAsyncEvent([this, packet, protocol]() {
		m_clientMutex.lock();
		for(auto &cl : m_clients)
			cl->SendPacket(packet, protocol);
		m_clientMutex.unlock();
	});
}
void nwm::Server::SendPacket(Protocol protocol, const NetPacket &packet, const RecipientFilter &rp)
{
#ifdef NWM_VERBOSE
	std::cout << "[NWMSV] Sending packet..." << std::endl;
#endif
	ScheduleAsyncEvent([this, packet, protocol, rp]() {
		m_clientMutex.lock();
		auto clients = GetFilterTargets(rp);
		for(auto *cl : clients)
			cl->SendPacket(packet, protocol);
		m_clientMutex.unlock();
	});
}

void nwm::Server::DropClient(ServerClient *client, ClientDropped reason, bool bDontTellClient)
{
#ifdef NWM_VERBOSE
	std::cout << "[NWMSV] Dropping client for reason '" << nwm::client_dropped_enum_to_string(reason) << "'..." << std::endl;
#endif
	if(client->m_bDropped == true)
		return;
	client->m_bDropped = true;
	ScheduleAsyncEvent([this, bDontTellClient, client, reason]() {
		if(bDontTellClient == false && client->IsReady()) {
			NetPacket p(umath::to_integral(MessageOut::Dropped));
			p->Write<int8_t>(static_cast<int8_t>(reason));
			client->SendPacket(p);
		}
		client->Close();

		SchedulePollEvent([this, client, reason]() -> PollEventResult {
			if(client->IsClosed() == false)
				return PollEventResult::Pending;
			std::vector<std::shared_ptr<ServerClient>> *clients;
			if(client->IsReady()) {
				OnClientDropped(client, reason);
				if(m_cbClientDropped != nullptr)
					m_cbClientDropped(client, reason);

				clients = &m_clients;
			}
			else
				clients = &m_clientsConnecting;
			auto it = std::find_if(clients->begin(), clients->end(), [client](const std::shared_ptr<nwm::ServerClient> &clientOther) { return (clientOther.get() == client) ? true : false; });
			assert(it != clients->end());
			if(it == clients->end())
				throw NWMException("[NWM] Attempted to remove invalid client from client list!");
			(*it)->Release();
			clients->erase(it);
			return PollEventResult::Complete;
		});
	});
}
void nwm::Server::SetClientDroppedHandle(const std::function<void(ServerClient *, ClientDropped)> &cbClientDropped) { m_cbClientDropped = cbClientDropped; }
void nwm::Server::SetClientHandle(const std::function<void(ServerClient *)> &cbClient) { m_cbClient = cbClient; }

/////////////////////////////

// These are NOT thread-safe!

void nwm::Server::UpdateClientReadyState(ServerClient *cl)
{
#ifdef NWM_VERBOSE
	std::cout << "[NWMSV] Updating client ready state..." << std::endl;
#endif
	if(cl->IsReady() == false)
		return;
	auto idx = cl->GetIndex();
	m_clients.push_back(std::move(m_clientsConnecting.at(idx)));
	RemoveClient(m_clientsConnecting, cl);
	cl->SetIndex(m_clients.size() - 1);

	auto hCl = std::make_shared<ServerClientHandle>(cl->GetHandle());
	SchedulePollEvent([this, hCl]() -> PollEventResult {
		if(hCl->IsValid() == false)
			return PollEventResult::Complete;
#ifdef NWM_VERBOSE
		std::cout << "[NWMSV] Client has connected successfully!" << std::endl;
#endif
		auto *cl = hCl->get();
		if(m_cbClient != nullptr)
			m_cbClient(cl);
		OnClientConnected(cl);

		ScheduleAsyncEvent([this, hCl]() {
			std::lock_guard<std::mutex> lg(m_clientMutex);
			if(hCl->IsValid() == false)
				return;
			NetPacket out(umath::to_integral(MessageOut::RegisterConfirmation));
			(*hCl)->SendPacket(out);
		});
		return PollEventResult::Complete;
	});
}
void nwm::Server::Run()
{
	m_clientMutex.lock();
	for(auto &cl : m_clients)
		cl->Run();
	m_clientMutex.unlock();
}

void nwm::Server::OnClientPacketSent(ServerClient *cl, const NetPacket &packet)
{
	auto ep = cl->GetRemoteEndpoint().Copy();
	OnPacketSent(ep, packet);
	/*SchedulePollEvent([this,ep,packet]() -> PollEventResult {
		OnPacketSent(ep,packet);
		return PollEventResult::Complete;
	});*/
}
bool nwm::Server::AcceptClient(NWMSession *session)
{
	auto address = session->GetAddress();
	auto maxClients = GetMaxClients();
#ifdef NWM_VERBOSE
	std::cout << "[NWMSV] Accepting new client '" << address.to_string() << "'..." << std::endl;
#endif
	if(IsIPBanned(address) || (maxClients != -1 && GetClientCount() >= maxClients)) {
#ifdef NWM_VERBOSE
		std::cout << "[NWMSV] Client was banned or max client count has been reached. Dropping client..." << std::endl;
#endif
		session->Close();
		session->Run(); // Make sure to terminate the session
		return false;
	}
	return true;
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
