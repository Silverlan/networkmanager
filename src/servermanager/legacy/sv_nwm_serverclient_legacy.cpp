// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "servermanager/legacy/sv_nwm_serverclient.h"
#include "servermanager/legacy/sv_nwm_manager.h"
#include "servermanager/legacy/sv_nwm_sessionhandle.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
extern NWMServer *server;
NWMServerClient::NWMServerClient(NWMServer *manager)
	: std::enable_shared_from_this<NWMServerClient>(),m_index(0),m_manager(manager),m_udpSession(nullptr),m_tcpSession(nullptr),
	m_bClosing(false),m_handle(),m_bClosed(false),m_latency(0),m_ip(nwm::invalid_address()),
	m_port(0),m_address{},m_bReady(false),m_bTCPInitialized(false),m_bUDPInitialized(false),
	m_bDropped(false)
{
	m_lastUpdate = util::Clock::now();
	m_handle = SessionHandle(this);
}

NWMServerClient::~NWMServerClient()
{
	if(m_udpSession != nullptr)
		m_udpSession->Release();
	if(m_tcpSession != nullptr)
		m_tcpSession->Release();
	m_handle.Invalidate();
}

unsigned short NWMServerClient::GetLatency() const {return m_latency;}
void NWMServerClient::SetLatency(unsigned short latency) {m_latency = latency;}

bool NWMServerClient::IsClosed() const {return m_bClosed;}

void NWMServerClient::Remove()
{
	if(m_udpSession != nullptr)
	{
		m_udpSession->Close();
		while(m_udpSession != nullptr && !m_udpSession->IsTerminated())
			m_udpSession->Run();
	}
	if(m_tcpSession != nullptr)
	{
		m_tcpSession->Close();
		while(m_tcpSession != nullptr && !m_tcpSession->IsTerminated())
			m_tcpSession->Run();
	}
}

SessionHandle NWMServerClient::GetHandle() {return m_handle;}
//SessionHandle *NWMServerClient::CreateHandle() {return m_handle.Copy();}

NWMServer *NWMServerClient::GetManager() const {return m_manager;}

ChronoDuration NWMServerClient::TimeSinceLastActivity() const
{
	m_lastUpdateMutex.lock();
		auto lastUpdate = m_lastUpdate;
	m_lastUpdateMutex.unlock();
	return std::chrono::duration_cast<ChronoDuration>(util::Clock::now() -lastUpdate);
}

void NWMServerClient::ResetLastUpdate()
{
	m_lastUpdateMutex.lock();
		m_lastUpdate = util::Clock::now();
	m_lastUpdateMutex.unlock();
}

void NWMServerClient::InitializeSessionData(NWMSession *session)
{
	if(m_udpSession != nullptr || m_tcpSession != nullptr)
		return;
	m_ip = session->GetIP();
	m_address = *session->GetAddress();
	m_port = session->GetPort();
	auto &ep = session->GetRemoteEndPoint();
	m_remoteEndPoint = ep.Copy();
}

template<class T>
	void NWMServerClient::SessionCloseHandle(std::shared_ptr<T> &t,bool bLock)
{
	bool bTimedOut = false;
	boost::system::error_code err {};
	if(t != nullptr)
	{
		bTimedOut = t->IsTimedOut();
		err = *t->GetLastError();
		t->Release();
		t = nullptr;
	}
	if(m_udpSession == nullptr && m_tcpSession == nullptr)
	{
		m_bClosed = true;
		OnClosed();
	}
	if(bTimedOut == true)
		m_manager->DropClient(this,CLIENT_DROPPED::TIMEOUT,true,false); // TODO lock?
	else if(err)
		m_manager->DropClient(this,CLIENT_DROPPED::ERROR,true,bLock);
}
void NWMServerClient::SetSession(NWMUDPSession *session)
{
	if(m_udpSession != nullptr)
		m_udpSession->Release();
	InitializeSessionData(session);
	m_udpSession = std::static_pointer_cast<NWMUDPSession>(session->shared_from_this());
	session->SetCloseHandle(std::bind(
		&NWMServerClient::SessionCloseHandle<NWMUDPSession>,this,std::ref(m_udpSession),false
	));
	m_handle.Initialize();
	m_bUDPInitialized = true;
	UpdateReadyState();
}
void NWMServerClient::SetSession(NWMTCPSession *session)
{
	if(m_tcpSession != nullptr)
		m_tcpSession->Release();
	InitializeSessionData(session);
	m_tcpSession = std::static_pointer_cast<NWMTCPSession>(session->shared_from_this());
	session->SetCloseHandle(std::bind(
		&NWMServerClient::SessionCloseHandle<NWMTCPSession>,this,std::ref(m_tcpSession),true
	));
	m_handle.Initialize();
	m_bTCPInitialized = true;
	UpdateReadyState();
}
void NWMServerClient::Drop(CLIENT_DROPPED reason)
{
	if(m_manager == nullptr)
		return;
	m_manager->DropClient(this,reason);
}

void NWMServerClient::UpdateReadyState()
{
	if(IsReady())
		return;
	bool bReady = false;
	if(m_manager->HasUDPConnection())
	{
		if(m_bUDPInitialized == true)
		{
			if(!m_manager->HasTCPConnection() || m_bTCPInitialized == true)
				bReady = true;
		}
	}
	else if(m_manager->HasTCPConnection() && m_bTCPInitialized == true)
		bReady = true;
	if(bReady == true)
	{
		if(m_udpSession != nullptr)
			m_udpSession->SetReady();
		if(m_tcpSession != nullptr)
			m_tcpSession->SetReady();
		m_bReady = true;
	}
}
bool NWMServerClient::IsReady() const {return m_bReady;}

void NWMServerClient::Close()
{
	m_bClosing = true;
}

void NWMServerClient::Terminate()
{
	if(m_udpSession != nullptr)
		m_udpSession->Close();
	if(m_tcpSession != nullptr)
		m_tcpSession->Close();
}

void NWMServerClient::Run()
{
	if(m_bClosing == true)
	{
		Terminate();
		m_bClosing = false;
	}
	if(m_udpSession != nullptr)
		m_udpSession->Run();
	if(m_tcpSession != nullptr)
		m_tcpSession->Run();
}

bool NWMServerClient::IsClosing() const {return m_bClosing;}
void NWMServerClient::SetIndex(size_t idx) {m_index = idx;}
size_t NWMServerClient::GetIndex() const {return m_index;}
void NWMServerClient::SetEndPoint(const NWMEndpoint&)
{
	// TODO What is this for, exactly?
	/*if(ep->IsUDP())
	{
		if(m_udpSession != nullptr)
			return;
		NWMUDPConnection *con = m_manager->GetUDPConnection();
		if(con == nullptr)
			return;
		m_udpSession = new NWMUDPSession(*con->ioService,ep);
		return;
	}
	if(m_tcpSession != nullptr)
		return;
	NWMTCPConnection *con = m_manager->GetTCPConnection();
	if(con == nullptr)
		return;
	m_tcpSession = new NWMTCPSession(*con->ioService,ep);*/
}
std::string NWMServerClient::GetIP() const {return m_ip;}
nwm::IPAddress NWMServerClient::GetAddress() const {return m_address;}
unsigned short NWMServerClient::GetPort() const {return m_port;}
const NWMEndpoint &NWMServerClient::GetRemoteEndpoint() const {return m_remoteEndPoint;}
std::string NWMServerClient::GetRemoteIP() const {return m_remoteEndPoint.GetIP();}
nwm::IPAddress NWMServerClient::GetRemoteAddress() const {return *m_remoteEndPoint.GetAddress();}
unsigned short NWMServerClient::GetRemotePort() const {return m_remoteEndPoint.GetPort();}
bool NWMServerClient::IsTarget(const NWMEndpoint &ep)
{
	if(ep->IsUDP())
	{
		if(m_udpSession == nullptr)
			return false;
		return (m_udpSession->GetRemoteEndPoint() == ep) ? true : false;
	}
	if(m_tcpSession == nullptr)
		return false;
	return (m_tcpSession->GetRemoteEndPoint() == ep) ? true : false;
}
bool NWMServerClient::IsTarget(const nwm::IPAddress &address,unsigned short port)
{
	if(m_tcpSession != nullptr)
		return (m_tcpSession->GetAddress() == address && m_tcpSession->GetPort() == port) ? true : false;
	if(m_udpSession == nullptr)
		return false;
	return (m_udpSession->GetAddress() == address && m_udpSession->GetPort() == port) ? true : false;
}

void NWMServerClient::SendPacket(const NetPacket &p,bool bPreferUDP)
{
	m_manager->OnClientPacketSent(this,p);
	if(bPreferUDP == true)
	{
		if(m_udpSession != nullptr)
		{
			m_udpSession->SendPacket(p);
			return;
		}
	}
	if(m_tcpSession != nullptr)
	{
		m_tcpSession->SendPacket(p);
		return;
	}
	if(m_udpSession == nullptr)
		return;
	m_udpSession->SendPacket(p);
}
void NWMServerClient::SendPacketTCP(const NetPacket &p) {SendPacket(p);}
void NWMServerClient::SendPacketUDP(const NetPacket &p) {SendPacket(p,true);}

bool NWMServerClient::IsUDPConnected() const {return (m_udpSession != nullptr) ? true : false;}
bool NWMServerClient::IsTCPConnected() const {return (m_tcpSession != nullptr) ? true : false;}
bool NWMServerClient::IsFullyConnected() const {return ((m_manager->HasUDPConnection() && !IsUDPConnected()) || (m_manager->HasTCPConnection() && !IsTCPConnected())) ? false : true;}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
