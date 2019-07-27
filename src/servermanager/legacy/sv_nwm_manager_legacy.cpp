/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "servermanager/legacy/sv_nwm_manager.h"
#include <sharedutils/netpacket.hpp>
#include <mathutil/umath.h>

/*NWMServer *NWMServer::Create(unsigned int masterServerPort,unsigned char conType)
{
	return Create<NWMServer>(masterServerPort,conType);
}*/

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NWMServer::NWMServer(const std::shared_ptr<SVNWMUDPConnection> &udp,const std::shared_ptr<SVNWMTCPConnection> &tcp)
	: NWManagerBase(udp,tcp),m_maxClients(-1)
{
	auto packetHandle = std::bind([](NWMServer *sv,const NWMEndpoint &ep,NWMIOBase *io,unsigned int id,NetPacket &readPacket) {
		NWMSession *session = dynamic_cast<NWMSession*>(io);
		//if(session == nullptr)
		//	return;
		NWMServerClient *svCl = (session != nullptr) ? sv->FindClient(session->GetRemoteEndPoint()) : nullptr;
		if(!sv->HandlePacket(ep,session,svCl,id,readPacket))
			sv->HandleError(NWMError(NETWORK_ERROR_UNHANDLED_PACKET,"Unhandled packet '" +std::to_string(id) +"'"));
	},this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4);
	auto clientHandle = [](NWMServer *sv,NWMSession *session) {
		auto clients = sv->GetAllClients();
		auto address = session->GetAddress();
		NWMServerClient *cl = nullptr;
		for(auto it=clients.begin();it!=clients.end();it++)
		{
			auto &client = *it;
			if(client.IsValid() && !client->IsClosed() && !client->IsClosing() && client->GetAddress() == address)
			{
				if(client->GetPort() == session->GetPort())
				{
					sv->DropClient(client.get(),CLIENT_DROPPED::TIMEOUT);
					return;
				}
				else
				{
					if((!client->IsUDPConnected() && session->IsUDP()) || (!client->IsTCPConnected() && session->IsTCP()))
					{
						cl = client.get();
						if(session->IsTCP() == true)
							cl->SetSession(static_cast<NWMTCPSession*>(session));
						else
							cl->SetSession(static_cast<NWMUDPSession*>(session));
						break;
					}
				}
			}
		}
		if(cl == nullptr)
		{
			sv->m_clientsIncoming.push_back(sv->CreateClient());
			cl = sv->m_clientsIncoming.back().get();
			if(session->IsTCP() == true)
				cl->SetSession(static_cast<NWMTCPSession*>(session));
			else
				cl->SetSession(static_cast<NWMUDPSession*>(session));
			cl->SetIndex(sv->m_clientsIncoming.size() -1);
		}
		if(cl->IsReady())
		{
			auto idx = cl->GetIndex();
			sv->m_clients.push_back(std::move(sv->m_clientsIncoming[idx]));
			sv->RemoveClient(sv->m_clientsIncoming,cl);
			cl->SetIndex(sv->m_clients.size() -1);
			
			if(sv->m_cbClient != nullptr)
				sv->m_cbClient(cl);
			NetPacket out(NWM_MESSAGE_OUT_REGISTER_CONFIRMATION);
			cl->SendPacket(out);

			auto ptrCl = cl->shared_from_this();
			sv->QueueEvent([sv,ptrCl]() {
				sv->OnClientConnected(ptrCl.get());
			});
		}
	};
	if(udp != nullptr)
	{
		//udp->SetServer(this);
		udp->SetPacketHandle(packetHandle);
		udp->SetClientHandle(std::bind(clientHandle,this,std::placeholders::_1));
	}
	if(tcp != nullptr)
	{
		//tcp->SetServer(this);
		tcp->SetPacketHandle(packetHandle);
		tcp->SetClientHandle(std::bind(clientHandle,this,std::placeholders::_1));
	}
}

NWMServer::~NWMServer()
{
	Close();
	while(IsActive())
		Run();
	m_clients.clear();
}

void NWMServer::RemoveClient(std::vector<std::shared_ptr<NWMServerClient>> &clients,NWMServerClient *cl)
{
	auto idx = cl->GetIndex();
	clients.erase(clients.begin() +idx);
	if(idx != (clients.size() +1))
	{
		for(auto i=idx;i<clients.size();i++)
			clients[i]->SetIndex(i);
	}
}

void NWMServer::SetMaxClients(int numClients) {m_maxClients = numClients;}
int NWMServer::GetMaxClients() const {return m_maxClients;}
int NWMServer::GetClientCount() const {return static_cast<int>(m_clients.size());}

bool NWMServer::AcceptClient(NWMSession *session)
{
	boost::asio::ip::address address = *session->GetAddress();
	auto maxClients = GetMaxClients();
	if(IsIPBanned(address) || (maxClients != -1 && GetClientCount() >= maxClients))
	{
		session->Close();
		session->Run(); // Make sure to terminate the session
		return false;
	}
	return true;
}

void NWMServer::Shutdown()
{
	if(!m_clients.empty())
	{
		size_t i = m_clients.size();
		while(i > 0)
		{
			i--;
			auto &client = m_clients[i];
			DropClient(client.get(),CLIENT_DROPPED::SHUTDOWN);
		}
	}
	Close();
	while(IsActive())
	{
		if(!IsAsync())
			Run();
	}
}

SessionHandle NWMServer::GetClient(const std::string &ip,unsigned short port)
{
	auto address = boost::asio::ip::address::from_string(ip);
	return GetClient(address,port);
}
SessionHandle NWMServer::GetClient(const boost::asio::ip::address &address,unsigned short port)
{
	for(auto it=m_clients.begin();it!=m_clients.end();it++)
	{
		auto &client = *it;
		if(client->IsTarget(address,port))
			return client->GetHandle();
	}
	return SessionHandle();
}

bool NWMServer::IsIPBanned(const std::string &ip)
{
	auto address = boost::asio::ip::address::from_string(ip);
	return IsIPBanned(address);
}

bool NWMServer::IsIPBanned(const boost::asio::ip::address &ip)
{
	for(auto it=m_banned.begin();it!=m_banned.end();it++)
	{
		if(*it == ip)
			return true;
	}
	return false;
}

void NWMServer::BanIP(const std::string &ip) {m_banned.push_back(boost::asio::ip::address::from_string(ip));} // TODO Kick, if already connected
void NWMServer::BanIPs(const std::vector<std::string> &ips)
{
	size_t offset = m_banned.size();
	m_banned.resize(offset +ips.size());
	for(size_t i=offset;i<(offset +ips.size());i++)
		m_banned[i] = boost::asio::ip::address::from_string(ips[i -offset]);
}

std::vector<NWMServerClient*> NWMServer::GetFilterTargets(const RecipientFilter &filter)
{
	std::vector<NWMServerClient*> clients;
	auto &sessions = const_cast<RecipientFilter&>(filter).get();
	if(filter.GetFilterType() == RPFilterType::INCLUDE)
	{
		for(auto it=sessions.begin();it!=sessions.end();it++)
		{
			SessionHandle &hSession = *it;
			if(hSession.IsValid())
				clients.push_back(hSession.get());
		}
		return clients;
	}
	for(auto it=m_clients.begin();it!=m_clients.end();it++)
	{
		bool bExists = false;
		auto &cl = *it;
		for(auto itFilter=sessions.begin();itFilter!=sessions.end();itFilter++)
		{
			SessionHandle &hSession = *itFilter;
			if(hSession.IsValid() && hSession.get() == cl.get())
			{
				bExists = true;
				break;
			}
		}
		if(bExists == false)
			clients.push_back(cl.get());
	}
	return clients;
}

void NWMServer::SendPacketTCP(const NetPacket &packet,const RecipientFilter &rp)
{
	auto clients = GetFilterTargets(rp);
	for(auto it=clients.begin();it!=clients.end();it++)
	{
		NWMServerClient *cl = *it;
		cl->SendPacketTCP(packet);
	}
}
void NWMServer::SendPacketUDP(const NetPacket &packet,const RecipientFilter &rp)
{
	auto clients = GetFilterTargets(rp);
	for(auto it=clients.begin();it!=clients.end();it++)
	{
		NWMServerClient *cl = *it;
		cl->SendPacketUDP(packet);
	}
}

void NWMServer::SetClientHandle(const std::function<void(NWMServerClient*)> &cbClient) {m_cbClient = cbClient;}

NWMServerClient *NWMServer::FindClient(const NWMEndpoint &ep)
{
	for(auto it=m_clients.begin();it!=m_clients.end();it++)
	{
		auto &svCl = *it;
		if(svCl->IsTarget(ep))
			return svCl.get();
	}
	return nullptr;
}

void NWMServer::SendPacket(const NetPacket&,NWMEndpoint&,bool) {}

void NWMServer::OnClientPacketSent(NWMServerClient *cl,const NetPacket &packet) {OnPacketSent(cl->GetRemoteEndpoint(),packet);}

void NWMServer::SetClientDroppedHandle(const std::function<void(NWMServerClient*,CLIENT_DROPPED)> &cbClientDropped) {m_cbClientDropped = cbClientDropped;}

void NWMServer::DropClient(NWMServerClient *client,CLIENT_DROPPED reason,bool bDontTellClient,bool bLock)
{
	if(client->m_bDropped == true)
		return;
	if(bLock == true)
		m_clientMutex.lock();
		client->m_bDropped = true;
		if(bDontTellClient == false && client->IsReady())
		{
			NetPacket p(NWM_MESSAGE_OUT_DROPPED);
			p->Write<char>(static_cast<char>(reason));
			client->SendPacket(p);
		}
		client->Close();
		m_callQueue.push_back([this,client,reason]() {
			if(!client->IsClosed())
				return false;
			std::vector<std::shared_ptr<NWMServerClient>> *clients;
			if(client->IsReady())
			{
				auto ptrCl = client->shared_from_this();
				QueueEvent([this,ptrCl,reason]() {
					OnClientDropped(ptrCl.get(),reason);
					if(m_cbClientDropped != nullptr)
						m_cbClientDropped(ptrCl.get(),reason);
				});
				
				clients = &m_clients;
			}
			else
				clients = &m_clientsIncoming;
			RemoveClient(*clients,client);
			return true;
		});
	if(bLock == true)
		m_clientMutex.unlock();
}

void NWMServer::ScheduleDropClient(NWMServerClient *client,CLIENT_DROPPED reason)
{
	auto hClient = client->GetHandle();
	m_callQueue.push_back([this,hClient,reason]() {
		if(!hClient.IsValid())
			return true;
		DropClient(hClient.get(),reason);
		return true;
	});
}

void NWMServer::Run()
{
	NWManagerBase::Run();
	std::lock_guard<std::mutex> mg(m_clientMutex);
	for(unsigned int i=0;i<m_callQueue.size();)
	{
		auto &f = m_callQueue[i];
		if(f() == true)
			m_callQueue.erase(m_callQueue.begin() +i);
		else
			i++;
	}
	std::vector<std::shared_ptr<NWMServerClient>> *clients[2] = {&m_clients,&m_clientsIncoming};
	for(char j=0;j<2;j++)
	{
		for(auto it=clients[j]->begin();it!=clients[j]->end();it++)
			(*it)->Run();
	}
	/*if(IsClosing())
		return;
	ChronoTimePoint now = util::Clock::now();
	if(std::chrono::duration_cast<std::chrono::seconds>(now -m_lastCheck).count() < NWM_SERVER_INACTIVITY_CHECK_INTERVAL)
		return;
	m_lastCheck = now;
	for(char j=0;j<2;j++)
	{
		for(auto it=clients[j]->begin();it!=clients[j]->end();it++)
		{
			NWMServerClient *client = *it;
			if(!client->IsClosing())
			{
				auto dur = std::chrono::duration_cast<std::chrono::seconds>(client->TimeSinceLastActivity()).count();
				//if(dur > NWM_SERVER_INACTIVITY_TOLERANCE)
				//	DropClient(client,CLIENT_DROPPED::TIMEOUT,false,false);
			}
		}
	}*/ // Check is now performed in IOBase
}
void NWMServer::BroadcastTCP(const NetPacket &packet)
{
	std::lock_guard<std::mutex> mg(m_clientMutex);
	for(auto it=m_clients.begin();it!=m_clients.end();it++)
	{
		auto &client = *it;
		client->SendPacketTCP(packet);
	}
}

void NWMServer::BroadcastUDP(const NetPacket &packet)
{
	std::lock_guard<std::mutex> mg(m_clientMutex);
	for(auto it=m_clients.begin();it!=m_clients.end();it++)
	{
		auto &client = *it;
		client->SendPacketUDP(packet);
	}
}

std::vector<SessionHandle> NWMServer::GetClients()
{
	std::vector<SessionHandle> sessions;
	GetClients(sessions);
	return sessions;
}

void NWMServer::GetClients(std::vector<SessionHandle> &clients)
{
	std::lock_guard<std::mutex> mg(m_clientMutex);
	clients.resize(m_clients.size());
	size_t i = 0;
	for(auto it=m_clients.begin();it!=m_clients.end();it++,i++)
		clients[i] = (*it)->GetHandle();
}

std::vector<SessionHandle> NWMServer::GetAllClients()
{
	std::vector<SessionHandle> sessions;
	GetAllClients(sessions);
	return sessions;
}

void NWMServer::GetAllClients(std::vector<SessionHandle> &clients)
{
	std::lock_guard<std::mutex> mg(m_clientMutex);
	clients.resize(m_clients.size() +m_clientsIncoming.size());
	size_t i = 0;
	for(auto it=m_clients.begin();it!=m_clients.end();it++,i++)
		clients[i] = (*it)->GetHandle();
	for(auto it=m_clientsIncoming.begin();it!=m_clientsIncoming.end();it++,i++)
		clients[i] = (*it)->GetHandle();
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
