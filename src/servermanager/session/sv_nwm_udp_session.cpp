/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "servermanager/session/sv_nwm_udp_session.h"
#include "servermanager/connection/sv_nwm_udpconnection.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NWMUDPSession::NWMUDPSession(const NWMEndpoint &ep,SVNWMUDPConnection *con)
	: NWMSession(),NWMIOBase(),NWMUDPIOBase(),m_connection(con),m_bReady(false)
{
	m_remoteEndpoint = ep.Copy();
	m_localEndpoint = con->GetLocalEndPoint();
}

NWMUDPSession::~NWMUDPSession()
{
}

void NWMUDPSession::InitializeSharedPtr()
{
	m_handle.ptr = std::dynamic_pointer_cast<NWMSession>(std::shared_ptr<NWMIOBase>(this));
}

void NWMUDPSession::OnPacketReceived()
{
	m_tLastMessage = std::chrono::high_resolution_clock::now();
}

void NWMUDPSession::Release()
{
	m_connection->RemoveSession(this);
	NWMSession::Release();
}

bool NWMUDPSession::IsTerminated() const {return NWMIOBase::IsTerminated() || NWMUDPIOBase::IsTerminated();}

const NWMEndpoint &NWMUDPSession::GetRemoteEndPoint() const {return NWMIOBase::GetRemoteEndPoint();}
const NWMEndpoint &NWMUDPSession::GetLocalEndPoint() const {return NWMIOBase::GetLocalEndPoint();}

bool NWMUDPSession::IsUDP() const {return true;}
void NWMUDPSession::SetPacketHandle(const std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)>&) {} // TODO NWMIO::SetPacketHandle(cbPacket);}
void NWMUDPSession::SetCloseHandle(const std::function<void(void)> &cbClose) {NWMIOBase::SetCloseHandle(cbClose);}

void NWMUDPSession::Close()
{
	if(m_closeHandle != nullptr)
		m_closeHandle();
}

std::string NWMUDPSession::GetIP() const
{
	const NWMEndpoint &ep = GetRemoteEndPoint();
	return ep.GetIP();
}
unsigned short NWMUDPSession::GetPort() const
{
	const NWMEndpoint &ep = GetRemoteEndPoint();
	return ep.GetPort();
}
boost::asio::ip::address NWMUDPSession::GetAddress() const
{
	const NWMEndpoint &ep = GetRemoteEndPoint();
	return ep.GetAddress();
}
std::string NWMUDPSession::GetLocalIP() const
{
	const auto &ep = GetLocalEndPoint();
	return ep.GetIP();
}
unsigned short NWMUDPSession::GetLocalPort() const
{
	const auto &ep = GetLocalEndPoint();
	return ep.GetPort();
}
boost::asio::ip::address NWMUDPSession::GetLocalAddress() const
{
	const auto &ep = GetLocalEndPoint();
	return ep.GetAddress();
}

void NWMUDPSession::Run() {NWMIOBase::Run();}

void NWMUDPSession::SendPacket(const NetPacket &packet) {m_connection->SendPacket(packet,GetRemoteEndPoint(),false);}

void NWMUDPSession::SetReady() {m_bReady = true;} // TODO
bool NWMUDPSession::IsReady() const {return m_bReady;}
bool NWMUDPSession::IsClosing() const {return false;} // TODO

void NWMUDPSession::SetTimeoutDuration(double duration) {NWMIOBase::SetTimeoutDuration(duration,true);}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
