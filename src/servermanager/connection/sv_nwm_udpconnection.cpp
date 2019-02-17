/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "servermanager/connection/sv_nwm_udpconnection.h"
#include "servermanager/legacy/sv_nwm_manager.h"
#include <sharedutils/scope_guard.h>

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
SVNWMUDPConnection::SVNWMUDPConnection(unsigned short localPort)
	: NWMUDPConnection(),SVNWMConnection(),NWMUDPIO(*ioService,localPort)
{
	NWMUDPIO::SetPacketHandle([this](const NWMEndpoint &ep,NWMIOBase*,unsigned int id,NetPacket &packet) {
		auto session = FindSession(static_cast<NWMUDPEndpoint&>(*ep));
		if(id == NWM_MESSAGE_IN_REGISTER_UDP)
		{
			auto *session = new NWMUDPSession(m_remoteEndpoint,this);
			session->Initialize();
			auto ptrSession = std::static_pointer_cast<NWMUDPSession>(session->shared_from_this());
			if(m_acceptCallbacks.lock != nullptr)
				m_acceptCallbacks.lock();
			ScopeGuard sg([this]() {
				if(m_acceptCallbacks.unlock != nullptr)
					m_acceptCallbacks.unlock();
			});
			if(m_acceptCallbacks.accept_client == nullptr || m_acceptCallbacks.accept_client(session) == false)
				return;
			AddSession(ptrSession);
			OnClientConnected(session);
			return;
		}
		//else if(session == nullptr) // TODO: Receive non-session messages
		//	return;
		if(session != nullptr)
			session->OnPacketReceived();
		m_packetSessionHandle(GetRemoteEndPoint(),static_cast<NWMIOBase*>(session.get()),id,packet);
	});
}
SVNWMUDPConnection::~SVNWMUDPConnection()
{}

void SVNWMUDPConnection::Initialize()
{
	Accept();
	SetReady();
}

void SVNWMUDPConnection::SetCloseHandle(const std::function<void(void)> &cbClose) {NWMUDPIO::SetCloseHandle(cbClose);}

void SVNWMUDPConnection::Run()
{
	NWMUDPConnection::Run();
	SVNWMConnection::Run();
	NWMUDPIO::Run();
}

void SVNWMUDPConnection::Accept()
{
	auto ep = udp::endpoint();
	m_remoteEndpoint = NWMEndpoint::CreateUDP(ep);
	AcceptNextFragment();
}

void SVNWMUDPConnection::SetTimeoutDuration(double duration) {SVNWMConnection::SetTimeoutDuration(duration);}

std::shared_ptr<NWMUDPSession> SVNWMUDPConnection::FindSession(const NWMUDPEndpoint &ep)
{
	auto *epUDP = const_cast<NWMUDPEndpoint&>(ep).get();
	if(epUDP == nullptr)
		return nullptr;
	auto *session = GetSession(*epUDP);
	if(session == nullptr)
		return nullptr;
	return std::static_pointer_cast<NWMUDPSession>(static_cast<NWMUDPSession*>(session)->shared_from_this());
}

void SVNWMUDPConnection::SendPacket(const NetPacket &packet,const NWMEndpoint &ep,bool bOwn) {NWMUDPIO::SendPacket(packet,ep,bOwn);}

bool SVNWMUDPConnection::HandleError(const boost::system::error_code &error)
{
	if(
		error == boost::asio::error::connection_refused ||
		error == boost::asio::error::connection_reset
	) // Client has become unreachable (Crashed?), in which case he will timeout anyway. No reason to close the socket.
		return true;
	return NWMUDPIO::HandleError(error);
}

void SVNWMUDPConnection::HandleReadHeader(const boost::system::error_code &err,std::size_t bytes)
{
	if(
		err == boost::asio::error::connection_refused ||
		err == boost::asio::error::connection_reset
	)
	{
		std::shared_ptr<NWMUDPSession> session = FindSession(static_cast<const NWMUDPEndpoint&>(*GetRemoteEndPoint()));
		if(session != nullptr)
			session->Close();
	}
	NWMUDPIO::HandleReadHeader(err,bytes);
}

bool SVNWMUDPConnection::IsClosing() const {return NWMIO::IsClosing();}
void SVNWMUDPConnection::Close() {NWMUDPIO::Close(),NWMUDPConnection::Close();}
void SVNWMUDPConnection::CloseSocket() {NWMUDPIO::CloseSocket();}
void SVNWMUDPConnection::ScheduleTermination() {NWMUDPIO::ScheduleTermination();}
void SVNWMUDPConnection::Terminate()
{
	NWMUDPConnection::Terminate();
	NWMUDPIO::Terminate();
}
bool SVNWMUDPConnection::IsTerminated() const {return NWMUDPIOBase::IsTerminated();}

void SVNWMUDPConnection::SetPacketHandle(const std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> &cbPacket) {m_packetSessionHandle = cbPacket;}

std::string SVNWMUDPConnection::GetLocalIP() const {return NWMUDPIO::GetLocalIP();}
unsigned short SVNWMUDPConnection::GetLocalPort() const {return NWMUDPIO::GetLocalPort();}
boost::asio::ip::address SVNWMUDPConnection::GetLocalAddress() const {return NWMUDPIO::GetLocalAddress();}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
