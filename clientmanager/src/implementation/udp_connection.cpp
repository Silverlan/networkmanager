// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

module pragma.client_manager;

import :enums;
import :udp_connection;
import pragma.network_manager;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
CLNWMUDPConnection::CLNWMUDPConnection(unsigned short localPort) : NWMUDPConnection(), CLNWMConnection(this), NWMUDPIO(*ioService, localPort) {}
CLNWMUDPConnection::~CLNWMUDPConnection() {}

std::string CLNWMUDPConnection::GetLocalIP() const { return NWMUDPIO::GetLocalIP(); }
unsigned short CLNWMUDPConnection::GetLocalPort() const { return NWMUDPIO::GetLocalPort(); }
nwm::IPAddress CLNWMUDPConnection::GetLocalAddress() const { return NWMUDPIO::GetLocalAddress(); }
void CLNWMUDPConnection::SetTimeoutDuration(double duration) { NWMUDPIO::SetTimeoutDuration(duration, false); }
bool CLNWMUDPConnection::IsClosing() const { return NWMIOBase::IsClosing(); }

void CLNWMUDPConnection::Connect(std::string serverIp, unsigned int serverPort)
{
	boost::asio::ip::udp::resolver resolver(**ioService);
	auto results = resolver.resolve(serverIp, std::to_string(serverPort));
	boost::asio::ip::udp::endpoint ep = *results.begin();

	m_remoteEndpoint = NWMEndpoint::CreateUDP(nwm::UDPEndpoint {&ep});
	SetReady();
	NetPacket out(NWM_MESSAGE_OUT_REGISTER_UDP);
	SendPacket(out);
}
void CLNWMUDPConnection::SetCloseHandle(const std::function<void(void)> &cbClose) { NWMUDPIO::SetCloseHandle(cbClose); }
void CLNWMUDPConnection::SetReady()
{
	AcceptNextFragment();
	NWMUDPIO::SetReady();
}

NWMUDPEndpoint *CLNWMUDPConnection::GetRemoteEndPoint() { return NWMIO::GetRemoteEndPoint<NWMUDPEndpoint>(); }

std::string CLNWMUDPConnection::GetIP() const { return ""; }

void CLNWMUDPConnection::CloseSocket() { NWMUDPIO::CloseSocket(); }
void CLNWMUDPConnection::Close()
{
	NWMUDPConnection::Close();
	NWMUDPIO::Close();
}
void CLNWMUDPConnection::SendPacket(const NetPacket &packet, bool bOwn) { SendPacket(packet, m_remoteEndpoint, bOwn); }
void CLNWMUDPConnection::SendPacket(const NetPacket &packet, const NWMEndpoint &ep, bool bOwn) { NWMIO::SendPacket(packet, ep, bOwn); }
void CLNWMUDPConnection::Run()
{
	NWMUDPConnection::Run();
	NWMUDPIO::Run();
}
void CLNWMUDPConnection::OnTimedOut() { CLNWMConnection::OnTimedOut(); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
