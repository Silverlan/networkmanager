// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>
#include "sharedutils/util_clock.hpp"

module pragma.server_manager;

import :session.tcp;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMTCPSession::NWMTCPSession(nwm::IOService &ioService, SVNWMTCPConnection *con) : NWMSession(), NWMTCPIO(ioService), m_connection(con) {}

NWMTCPSession::~NWMTCPSession() {}

void NWMTCPSession::OnPacketReceived() { m_tLastMessage = util::Clock::now(); }

void NWMTCPSession::Release()
{
	if(m_handle.expired())
		return;
	m_connection->RemoveSession(this);
	NWMSession::Release();
}

bool NWMTCPSession::IsConnectionActive()
{
	if(!nwm::cast_socket(*socket)->is_open())
		return false;
	return NWMTCPIO::IsConnectionActive();
}

bool NWMTCPSession::IsUDP() const { return false; }
std::string NWMTCPSession::GetIP() const { return NWMTCPIO::GetIP(); }
unsigned short NWMTCPSession::GetPort() const { return NWMTCPIO::GetPort(); }
nwm::IPAddress NWMTCPSession::GetAddress() const { return NWMTCPIO::GetAddress(); }

void NWMTCPSession::OnTerminated()
{
	NWMTCPIO::OnTerminated();
	Release();
}

void NWMTCPSession::InitializeSharedPtr() { m_handle.SetPointer(std::dynamic_pointer_cast<NWMSession>(std::shared_ptr<NWMIOBase>(this))); }

const NWMEndpoint &NWMTCPSession::GetRemoteEndPoint() const { return NWMIO::GetRemoteEndPoint(); }

void NWMTCPSession::SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket) { NWMIO::SetPacketHandle(cbPacket); }
void NWMTCPSession::SetCloseHandle(const std::function<void(void)> &cbClose) { NWMIO::SetCloseHandle(cbClose); }

void NWMTCPSession::Start()
{
	boost::asio::ip::tcp::endpoint ep = nwm::cast_socket(*socket)->remote_endpoint();
	m_remoteEndpoint = NWMEndpoint::CreateTCP(nwm::TCPEndpoint {&ep});
	AcceptNextFragment();
}

void NWMTCPSession::Close() { NWMTCPIO::Close(); }

void NWMTCPSession::Run() { NWMTCPIO::Run(); }

void NWMTCPSession::SendPacket(const NetPacket &packet) { NWMTCPIO::SendPacket(packet); }

void NWMTCPSession::SetTimeoutDuration(double duration) { NWMTCPIO::SetTimeoutDuration(duration, true); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
