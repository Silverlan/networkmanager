// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "servermanager/connection/sv_nwm_tcpconnection.h"
#include "servermanager/interface/sv_nwm_manager.hpp"
#include "servermanager/wrappers/sv_nwm_impl_boost.hpp"
#include <networkmanager/wrappers/nwm_boost_wrapper_impl.hpp>
#include <sharedutils/scope_guard.h>

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
SVNWMTCPConnection::SVNWMTCPConnection(unsigned short minPort,unsigned short maxPort)
	: NWMTCPConnection(),SVNWMConnection(),NWMErrorHandle(),NWMEventBase(),
	std::enable_shared_from_this<SVNWMTCPConnection>(),m_acceptor(nullptr),
	m_bClosing(false),m_bTerminating(false),m_bNagleEnabled(false)
{
	auto port = minPort;
	const char *msg = nullptr;
	while(m_acceptor == nullptr && port <= maxPort)
	{
		try
		{
#if NWM_USE_IPV6 == 0
			endPoint = NWMEndpoint::CreateTCP(tcp::v4(),port);
#else
			endPoint = NWMEndpoint::CreateTCP(tcp::v6(),port);
#endif
			auto &ep = *static_cast<NWMTCPEndpoint*>(endPoint.get())->get();
			m_acceptor = std::make_unique<nwm::TCPAcceptor>(*ioService,ep);
		}
		catch(std::exception e)
		{
			port++;
			msg = e.what();
		}
	}
	if(m_acceptor == nullptr)
	{
		ioService = nullptr;
		std::string err = "Unable to open TCP acceptor on port ";
		err += std::to_string(port);
		if(msg != nullptr)
		{
			err += ": ";
			err += msg;
		}
		throw NWMException(err);
	}
	else
		nwm::cast_acceptor(*m_acceptor)->set_option(tcp::no_delay(m_bNagleEnabled));
}

SVNWMTCPConnection::SVNWMTCPConnection(unsigned short port)
	: SVNWMTCPConnection(port,port)
{}

SVNWMTCPConnection::~SVNWMTCPConnection()
{
	if(m_acceptor != nullptr)
	{
		if(nwm::cast_acceptor(*m_acceptor)->is_open())
			nwm::cast_acceptor(*m_acceptor)->close();
		m_acceptor = nullptr;
	}
}

void SVNWMTCPConnection::SetNagleAlgorithmEnabled(bool b)
{
	m_bNagleEnabled = b;
	if(m_acceptor != nullptr)
		nwm::cast_acceptor(*m_acceptor)->set_option(tcp::no_delay(m_bNagleEnabled));
	/*for(auto it=m_sessions.begin();it!=m_sessions.end();++it)
	{
		if(it->IsValid())
		{
			auto *session = it->get<NWMTCPSession>();
			session->SetNagleAlgorithmEnabled(b);
		}
	}*/
}

void SVNWMTCPConnection::Initialize()
{
	Accept();
}

void SVNWMTCPConnection::SetTimeoutDuration(double duration) {SVNWMConnection::SetTimeoutDuration(duration);}

void SVNWMTCPConnection::OnClientConnected(NWMSession *session)
{
	session->SetPacketHandle(m_packetHandle);
	SVNWMConnection::OnClientConnected(session);
}

void SVNWMTCPConnection::SetCloseHandle(const std::function<void(void)> &cbClose) {m_closeHandle = cbClose;}
void SVNWMTCPConnection::SetPacketHandle(const std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> &cbPacket)
{
	m_packetHandle = cbPacket;
	for(auto it=m_sessions.begin();it!=m_sessions.end();it++)
		(*it)->SetPacketHandle(cbPacket);
}

void SVNWMTCPConnection::HandleAccept(NWMSessionHandle hSession,const nwm::ErrorCode &err)
{
	if(!hSession.IsValid() || HandleError(err) == false)
		return;
	auto *session = hSession.get<NWMTCPSession>();
	//session->SetNagleAlgorithmEnabled(m_bNagleEnabled);
	session->InitializeSocket();
	Accept();
	ScheduleEvent([this,session]() {
		if(m_acceptCallbacks.lock != nullptr)
			m_acceptCallbacks.lock();
		util::ScopeGuard sg([this]() {
			if(m_acceptCallbacks.unlock != nullptr)
				m_acceptCallbacks.unlock();
		});
		auto address = session->GetAddress();
		auto port = session->GetPort();
		if(m_acceptCallbacks.has_client != nullptr && m_acceptCallbacks.has_client(address,port) == true)
		{
			session->Close();
			session->Run();
			session->SetNagleAlgorithmEnabled(m_bNagleEnabled);
			m_sessions.pop_back();
			return;
		}
		if(m_acceptCallbacks.accept_client == nullptr || m_acceptCallbacks.accept_client(session) == false)
			return;
		session->Start();
		OnClientConnected(session);
	});
}

void SVNWMTCPConnection::ScheduleTermination() {m_bTerminating = true; m_bClosing = true;}

void SVNWMTCPConnection::Terminate()
{
	if(m_acceptor != nullptr && nwm::cast_acceptor(*m_acceptor)->is_open())
		nwm::cast_acceptor(*m_acceptor)->close();
	NWMTCPConnection::Terminate();
	if(m_closeHandle != nullptr)
		m_closeHandle();
}

void SVNWMTCPConnection::SendPacket(const NetPacket&,const NWMEndpoint&,bool) {}

void SVNWMTCPConnection::Accept()
{
	if(IsClosing()) return;
	auto *session = new NWMTCPSession(*ioService,this);
	session->Initialize();
	auto ptrSession = std::static_pointer_cast<NWMTCPSession>(session->shared_from_this());
	auto hSession = AddSession(ptrSession);
	nwm::cast_acceptor(*m_acceptor)->async_accept(
		*nwm::cast_socket(*session->socket),
		[this,hSession](const boost::system::error_code &err) {
			HandleAccept(hSession,err);
		}
	);
}

bool SVNWMTCPConnection::IsClosing() const {return m_bClosing;}

void SVNWMTCPConnection::Run()
{
	NWMEventBase::ProcessEvents();
	if(IsClosing() && m_bTerminating == true)
	{
		Terminate();
		return;
	}
	NWMTCPConnection::Run();
	SVNWMConnection::Run();
}

void SVNWMTCPConnection::Close()
{
	if(IsClosing())
		return;
	m_bClosing = true;
	NWMTCPConnection::Close();
	for(auto it=m_sessions.begin();it!=m_sessions.end();it++)
	{
		auto session = *it;
		session->Close();
	}
	AttemptTermination();
}

std::string SVNWMTCPConnection::GetLocalIP() const
{
	if(!IsActive())
		return nwm::invalid_address();
	return nwm::to_string(nwm::cast_acceptor(*m_acceptor)->local_endpoint());
}
unsigned short SVNWMTCPConnection::GetLocalPort() const
{
	if(!IsActive())
		return 0;
	auto endPoint = nwm::cast_acceptor(*m_acceptor)->local_endpoint();
	return endPoint.port();
}
nwm::IPAddress SVNWMTCPConnection::GetLocalAddress() const
{
	if(!IsActive())
		return boost::asio::ip::address();
	auto endPoint = nwm::cast_acceptor(*m_acceptor)->local_endpoint();
	return endPoint.address();
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
