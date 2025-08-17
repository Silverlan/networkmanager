// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "servermanager/interface/sv_nwm_serverclient.hpp"
#include "servermanager/session/sv_nwm_udp_session.h"
#include "servermanager/session/sv_nwm_tcp_session.h"
#include <networkmanager/wrappers/nwm_boost_wrapper_impl.hpp>
#include <networkmanager/wrappers/nwm_boost_wrapper_impl.hpp>

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
nwm::ServerClient::ServerClient(Server *manager) : std::enable_shared_from_this<nwm::ServerClient>(), m_manager(manager)
{
	boost::asio::ip::udp::endpoint udpEp = boost::asio::ip::basic_endpoint<boost::asio::ip::udp> {};
	NWMUDPEndpoint udpEndpoint {&udpEp};
	boost::asio::ip::tcp::endpoint tcpEp = boost::asio::ip::basic_endpoint<boost::asio::ip::tcp> {};
	NWMTCPEndpoint tcpEndpoint {&tcpEp};
	m_conUDP.remoteEndpoint = NWMEndpoint(udpEndpoint);
	m_conTCP.remoteEndpoint = NWMEndpoint(tcpEndpoint);
	m_lastUpdate = util::Clock::now();
	m_handle = ServerClientHandle(this);
}

nwm::ServerClient::~ServerClient()
{
	// Server manager needs to have called 'Release' on this object before it is destroyed!
	assert(m_udpSession == nullptr);
	assert(m_tcpSession == nullptr);
	m_sessionDataMutex.lock();
	m_handle.Invalidate();
	m_sessionDataMutex.unlock();
}

uint16_t nwm::ServerClient::GetLatency() const { return m_latency; }
void nwm::ServerClient::SetLatency(uint16_t latency) { m_latency = latency; }
bool nwm::ServerClient::IsClosed() const { return m_bClosed; }

nwm::ServerClientHandle nwm::ServerClient::GetHandle() const
{
	m_sessionDataMutex.lock();
	auto h = m_handle;
	m_sessionDataMutex.unlock();
	return h;
}
nwm::Server *nwm::ServerClient::GetManager() const { return m_manager; }

ChronoDuration nwm::ServerClient::TimeSinceLastActivity() const
{
	m_lastUpdateMutex.lock();
	auto lastUpdate = m_lastUpdate;
	m_lastUpdateMutex.unlock();
	return std::chrono::duration_cast<ChronoDuration>(util::Clock::now() - lastUpdate);
}

void nwm::ServerClient::ResetLastUpdate()
{
	m_lastUpdateMutex.lock();
	m_lastUpdate = util::Clock::now();
	m_lastUpdateMutex.unlock();
}

void nwm::ServerClient::Drop(ClientDropped e) { m_manager->DropClient(this, e); }
bool nwm::ServerClient::IsReady() const { return m_bReady; }
void nwm::ServerClient::Close() { m_bClosing = true; }

bool nwm::ServerClient::IsClosing() const { return m_bClosing; }
void nwm::ServerClient::SetIndex(size_t idx) { m_index = idx; }
size_t nwm::ServerClient::GetIndex() const { return m_index; }
std::string nwm::ServerClient::GetIP() const
{
	m_sessionDataMutex.lock();
	auto ip = m_conTCP.ip;
	m_sessionDataMutex.unlock();
	return ip;
}
void nwm::ServerClient::SetAddress(nwm::Protocol protocol, const nwm::IPAddress &address, uint16_t port)
{
	m_sessionDataMutex.lock();
	switch(protocol) {
	case Protocol::TCP:
		m_conTCP.address = address;
		m_conTCP.port = port;
		break;
	case Protocol::UDP:
		m_conUDP.address = address;
		m_conUDP.port = port;
		break;
	}
	m_sessionDataMutex.unlock();
}
nwm::IPAddress nwm::ServerClient::GetAddress() const
{
	m_sessionDataMutex.lock();
	auto address = m_conTCP.address;
	m_sessionDataMutex.unlock();
	return address;
}
uint16_t nwm::ServerClient::GetPort() const { return m_conTCP.port; }
NWMEndpoint nwm::ServerClient::GetRemoteEndpoint() const
{
	m_sessionDataMutex.lock();
	auto ep = m_conTCP.remoteEndpoint;
	m_sessionDataMutex.unlock();
	return ep;
}
std::string nwm::ServerClient::GetRemoteIP() const
{
	m_sessionDataMutex.lock();
	auto ip = m_conTCP.remoteEndpoint.GetIP();
	m_sessionDataMutex.unlock();
	return ip;
}
nwm::IPAddress nwm::ServerClient::GetRemoteAddress() const
{
	m_sessionDataMutex.lock();
	auto address = m_conTCP.remoteEndpoint.GetAddress();
	m_sessionDataMutex.unlock();
	return address;
}
uint16_t nwm::ServerClient::GetRemotePort() const
{
	m_sessionDataMutex.lock();
	auto port = m_conTCP.remoteEndpoint.GetPort();
	m_sessionDataMutex.unlock();
	return port;
}
bool nwm::ServerClient::IsTarget(const NWMEndpoint &ep) const
{
	std::lock_guard<std::mutex> lg(m_sessionDataMutex);
	if(IsTCPConnected() && ep == m_conTCP.remoteEndpoint)
		return true;
	if(IsUDPConnected() && ep == m_conUDP.remoteEndpoint)
		return true;
	return false;
}
bool nwm::ServerClient::IsTarget(const nwm::IPAddress &address, uint16_t port) const
{
	std::lock_guard<std::mutex> lg(m_sessionDataMutex);
	if(IsTCPConnected() && m_conTCP.address == address && m_conTCP.port == port)
		return true;
	if(IsUDPConnected() && m_conUDP.address == address && m_conUDP.port == port)
		return true;
	return false;
}
bool nwm::ServerClient::UsesAddress(const nwm::IPAddress &address) const
{
	std::lock_guard<std::mutex> lg(m_sessionDataMutex);
	if(m_conUDP.address == address)
		return true;
	if(m_conTCP.address == address)
		return true;
	return false;
}
bool nwm::ServerClient::UsesPort(uint16_t port) const
{
	if(IsUDPConnected() && m_conUDP.port == port)
		return true;
	if(IsTCPConnected() && m_conTCP.port == port)
		return true;
	return false;
}

bool nwm::ServerClient::IsUDPConnected() const { return m_conUDP.connected; }
bool nwm::ServerClient::IsTCPConnected() const { return m_conTCP.connected; }
bool nwm::ServerClient::IsFullyConnected() const { return ((m_manager->HasUDPConnection() && !IsUDPConnected()) || (m_manager->HasTCPConnection() && !IsTCPConnected())) ? false : true; }

// The following mustn't be called from main thread!
template<class T>
void nwm::ServerClient::SessionCloseHandle(std::shared_ptr<T> &t)
{
	bool bTimedOut = false;
	boost::system::error_code err {};
	if(t != nullptr) {
		bTimedOut = t->IsTimedOut();
		auto pErr = t->GetLastError();
		if(pErr)
			err = *pErr;
		t->Release();
		t = nullptr;
	}
	if(m_udpSession == nullptr && m_tcpSession == nullptr) {
		m_bClosed = true;
		OnClosed();
	}
	if(bTimedOut == true)
		m_manager->DropClient(this, ClientDropped::Timeout, true);
	else if(err)
		m_manager->DropClient(this, ClientDropped::Error, true);
}
void nwm::ServerClient::Release()
{
	if(m_udpSession != nullptr) {
		m_udpSession->Close();
		while(m_udpSession != nullptr && !m_udpSession->IsTerminated())
			m_udpSession->Run();
		m_udpSession = nullptr;
	}
	if(m_tcpSession != nullptr) {
		m_tcpSession->Close();
		while(m_tcpSession != nullptr && !m_tcpSession->IsTerminated())
			m_tcpSession->Run();
		m_tcpSession = nullptr;
	}
}
void nwm::ServerClient::Run()
{
	if(m_bClosing == true) {
		Terminate();
		m_bClosing = false;
	}
	if(m_udpSession != nullptr)
		m_udpSession->Run();
	if(m_tcpSession != nullptr)
		m_tcpSession->Run();
}
void nwm::ServerClient::Terminate()
{
	if(m_udpSession != nullptr)
		m_udpSession->Close();
	if(m_tcpSession != nullptr)
		m_tcpSession->Close();
}
void nwm::ServerClient::SendPacket(const NetPacket &p, nwm::Protocol protocol)
{
#if NWM_VERBOSE >= 2
	std::cout << "[NWMSV] Sending packet " << p.GetMessageID() << " to client " << GetIP() << " via " << nwm::protocol_enum_to_string(protocol) << " protocol..." << std::endl;
#endif
	m_manager->OnClientPacketSent(this, p);
	if(protocol == nwm::Protocol::UDP) {
		if(m_udpSession != nullptr) {
#if NWM_VERBOSE >= 2
			std::cout << "[NWMSV] Sending UDP packet..." << std::endl;
#endif
			m_udpSession->SendPacket(p);
			return;
		}
	}
	if(m_tcpSession != nullptr) {
#if NWM_VERBOSE >= 2
		std::cout << "[NWMSV] Sending TCP packet..." << std::endl;
#endif
		m_tcpSession->SendPacket(p);
		return;
	}
	if(m_udpSession == nullptr) {
#if NWM_VERBOSE >= 2
		std::cout << "[NWMSV] Unable to send packet: No TCP or UDP session available!" << std::endl;
#endif
		return;
	}
#if NWM_VERBOSE >= 2
	std::cout << "[NWMSV] Sending UDP packet..." << std::endl;
#endif
	m_udpSession->SendPacket(p);
}
void nwm::ServerClient::InitializeSessionData(NWMSession *session)
{
	auto &ep = session->GetRemoteEndPoint();
	auto &con = session->IsTCP() ? m_conTCP : m_conUDP;
	con.remoteEndpoint = ep.Copy();
	con.ip = session->GetIP();
	//con.address = session->GetAddress();
	//con.port = session->GetPort();
}
void nwm::ServerClient::SetSession(NWMUDPSession *session)
{
	m_sessionDataMutex.lock();
	if(m_udpSession != nullptr)
		m_udpSession->Release();
	InitializeSessionData(session);

	m_udpSession = std::static_pointer_cast<NWMUDPSession>(session->shared_from_this());
	session->SetCloseHandle(std::bind(&nwm::ServerClient::SessionCloseHandle<NWMUDPSession>, this, std::ref(m_udpSession)));
	m_handle.Initialize();
	m_conUDP.initialized = true;
	UpdateReadyState();
	m_conUDP.connected = true;
	m_sessionDataMutex.unlock();
}
void nwm::ServerClient::SetSession(NWMTCPSession *session)
{
	m_sessionDataMutex.lock();
	if(m_tcpSession != nullptr)
		m_tcpSession->Release();
	InitializeSessionData(session);

	m_tcpSession = std::static_pointer_cast<NWMTCPSession>(session->shared_from_this());
	session->SetCloseHandle(std::bind(&nwm::ServerClient::SessionCloseHandle<NWMTCPSession>, this, std::ref(m_tcpSession)));
	m_handle.Initialize();
	m_conTCP.initialized = true;
	UpdateReadyState();
	m_conTCP.connected = true;
	m_sessionDataMutex.unlock();
}

void nwm::ServerClient::UpdateReadyState()
{
	if(IsReady())
		return;
	auto bReady = false;
	if(m_manager->HasUDPConnection()) {
		if(m_conUDP.initialized == true) {
			if(!m_manager->HasTCPConnection() || m_conTCP.initialized == true)
				bReady = true;
		}
	}
	else if(m_manager->HasTCPConnection() && m_conTCP.initialized == true)
		bReady = true;
	if(bReady == true) {
		if(m_udpSession != nullptr)
			m_udpSession->SetReady();
		if(m_tcpSession != nullptr)
			m_tcpSession->SetReady();
		m_bReady = true;
	}
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
