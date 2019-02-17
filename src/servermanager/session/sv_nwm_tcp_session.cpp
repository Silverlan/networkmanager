/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "servermanager/session/sv_nwm_tcp_session.h"
#include "servermanager/connection/sv_nwm_tcpconnection.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NWMTCPSession::NWMTCPSession(boost::asio::io_service& ioService,SVNWMTCPConnection *con)
	: NWMSession(),NWMTCPIO(ioService),m_connection(con)
{}

NWMTCPSession::~NWMTCPSession()
{}

void NWMTCPSession::OnPacketReceived()
{
	m_tLastMessage = std::chrono::high_resolution_clock::now();
}

void NWMTCPSession::Release()
{
	if(m_handle.ptr == nullptr)
		return;
	m_connection->RemoveSession(this);
	NWMSession::Release();
}

bool NWMTCPSession::IsConnectionActive()
{
	if(!socket->is_open())
		return false;
	return NWMTCPIO::IsConnectionActive();
}

bool NWMTCPSession::IsUDP() const {return false;}
std::string NWMTCPSession::GetIP() const {return NWMTCPIO::GetIP();}
unsigned short NWMTCPSession::GetPort() const {return NWMTCPIO::GetPort();}
boost::asio::ip::address NWMTCPSession::GetAddress() const {return NWMTCPIO::GetAddress();}

void NWMTCPSession::OnTerminated()
{
	NWMTCPIO::OnTerminated();
	Release();
}

void NWMTCPSession::InitializeSharedPtr()
{
	m_handle.ptr = std::dynamic_pointer_cast<NWMSession>(std::shared_ptr<NWMIOBase>(this));
}

const NWMEndpoint &NWMTCPSession::GetRemoteEndPoint() const {return NWMIO::GetRemoteEndPoint();}

void NWMTCPSession::SetPacketHandle(const std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> &cbPacket) {NWMIO::SetPacketHandle(cbPacket);}
void NWMTCPSession::SetCloseHandle(const std::function<void(void)> &cbClose) {NWMIO::SetCloseHandle(cbClose);}

void NWMTCPSession::Start()
{
	auto ep = socket->remote_endpoint();
	m_remoteEndpoint = NWMEndpoint::CreateTCP(ep);
	AcceptNextFragment();
}

void NWMTCPSession::Close()
{
	NWMTCPIO::Close();
}

void NWMTCPSession::Run() {NWMTCPIO::Run();}

void NWMTCPSession::SendPacket(const NetPacket &packet) {NWMTCPIO::SendPacket(packet);}

void NWMTCPSession::SetTimeoutDuration(double duration) {NWMTCPIO::SetTimeoutDuration(duration,true);}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
