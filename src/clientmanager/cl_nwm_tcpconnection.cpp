/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "clientmanager/cl_nwm_tcpconnection.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
CLNWMTCPConnection::CLNWMTCPConnection()
	: NWMTCPConnection(),CLNWMConnection(this),NWMTCPIO(*ioService)
{}
CLNWMTCPConnection::~CLNWMTCPConnection()
{}
void CLNWMTCPConnection::SetConnectCallback(const std::function<void(void)> &f) {m_connectCallback = f;}
std::string CLNWMTCPConnection::GetLocalIP() const {return NWMTCPIO::GetLocalIP();}
unsigned short CLNWMTCPConnection::GetLocalPort() const {return NWMTCPIO::GetLocalPort();}
nwm::IPAddress CLNWMTCPConnection::GetLocalAddress() const {return NWMTCPIO::GetLocalAddress();}
void CLNWMTCPConnection::SetTimeoutDuration(double duration) {NWMTCPIO::SetTimeoutDuration(duration,false);}
NWMTCPEndpoint *CLNWMTCPConnection::GetRemoteEndPoint() {return NWMIO::GetRemoteEndPoint<NWMTCPEndpoint>();}
void CLNWMTCPConnection::SetNagleAlgorithmEnabled(bool b) {NWMTCPIO::SetNagleAlgorithmEnabled(b);}
void CLNWMTCPConnection::OnConnect(const nwm::ErrorCode&)
{
	InitializeSocket();
	SetReady();
	if(m_connectCallback != nullptr)
		m_connectCallback();
	NetPacket out(NWM_MESSAGE_OUT_REGISTER_TCP);
	SendPacket(out);
}
void CLNWMTCPConnection::SetReady()
{
	AcceptNextFragment();
	NWMTCPIO::SetReady();
}
void CLNWMTCPConnection::SetCloseHandle(const std::function<void(void)> &cbClose) {NWMTCPIO::SetCloseHandle(cbClose);}
void CLNWMTCPConnection::SendPacket(const NetPacket &packet,bool bOwn) {SendPacket(packet,m_remoteEndpoint,bOwn);}
void CLNWMTCPConnection::SendPacket(const NetPacket &packet,const NWMEndpoint &ep,bool bOwn) {NWMIO::SendPacket(packet,ep,bOwn);}
void CLNWMTCPConnection::HandleOnConnect(const nwm::ErrorCode &err)
{
	if(HandleError(err) == false)
		return;
#ifdef __linux__
	OnConnect(err);
#else
	ScheduleEvent(std::bind(&CLNWMTCPConnection::OnConnect,this,err));
#endif
}
void CLNWMTCPConnection::Connect(std::string serverIp,unsigned short serverPort)
{
	tcp::resolver res(**ioService);
	tcp::resolver::query q(serverIp,std::to_string(serverPort));
	tcp::resolver::iterator it = res.resolve(q);
	tcp::endpoint ep = *it;
	m_remoteEndpoint = NWMEndpoint::CreateTCP(nwm::TCPEndpoint{&ep});
	boost::asio::async_connect(*nwm::cast_socket(*socket),it,[this](const boost::system::error_code &err,boost::asio::ip::tcp::resolver::iterator it) {
		HandleOnConnect(err);
	});
}
void CLNWMTCPConnection::CloseSocket() {NWMTCPConnection::CloseSocket();}
bool CLNWMTCPConnection::IsClosing() const {return NWMTCPIO::IsClosing();}
void CLNWMTCPConnection::Close() {NWMTCPConnection::Close(); NWMTCPIO::Close();}
void CLNWMTCPConnection::Run()
{
	NWMTCPConnection::Run();
	NWMTCPIO::Run();
}
void CLNWMTCPConnection::OnTimedOut() {CLNWMConnection::OnTimedOut();}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
