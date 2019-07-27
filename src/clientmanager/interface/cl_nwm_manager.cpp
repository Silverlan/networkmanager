/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "clientmanager/interface/cl_nwm_manager.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
decltype(nwm::Client::PING_INTERVAL) nwm::Client::PING_INTERVAL = 2;
nwm::Client::Client(const std::shared_ptr<CLNWMUDPConnection> &udp,const std::shared_ptr<CLNWMTCPConnection> &tcp)
	: impl::ManagerBase(udp,tcp)
{}
nwm::Client::~Client()
{
	Close();
}
CLNWMUDPConnection *nwm::Client::GetUDPConnection() {return static_cast<CLNWMUDPConnection*>(ManagerBase::GetUDPConnection());}
CLNWMTCPConnection *nwm::Client::GetTCPConnection() {return static_cast<CLNWMTCPConnection*>(ManagerBase::GetTCPConnection());}
void nwm::Client::Initialize(const std::string &serverIp,uint16_t serverPort)
{
#ifdef NWM_VERBOSE
		std::cout<<"[NWMCL] Initializing client connection to "<<serverIp<<":"<<serverPort<<"..."<<std::endl;
#endif
	auto *udp = GetUDPConnection();
	auto *tcp = GetTCPConnection();
	std::fill(m_latencies.begin(),m_latencies.end(),0);
	auto packetHandle = std::bind([](nwm::Client *cl,const NWMEndpoint&,NWMIOBase *io,uint32_t id,NetPacket &readPacket) {
		auto ep = io->GetRemoteEndPoint();
		cl->SchedulePollEvent([cl,ep,id,readPacket]() mutable -> PollEventResult {
			if(cl->HandlePacket(ep,id,readPacket) == false)
				cl->HandleError(nwm::Error(umath::to_integral(nwm::Result::UnhandledPacket),"Unhandled packet '" +std::to_string(id) +'\''));
			return PollEventResult::Complete;
		});
	},this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4);
	if(udp != nullptr)
	{
#ifdef NWM_VERBOSE
		std::cout<<"[NWMCL] Initializing UDP connection..."<<std::endl;
#endif
		udp->SetPacketHandle(packetHandle);
		udp->SetConnectionHandle([this]() {
#ifdef NWM_VERBOSE
			std::cout<<"[NWMCL] UDP connection has been established!"<<std::endl;
#endif
			SchedulePollEvent([this]() -> PollEventResult {
				OnConnected();
				return PollEventResult::Complete;
			});
		});
		udp->SetDisconnectionHandle([this](nwm::ClientDropped reason) {
#ifdef NWM_VERBOSE
			std::cout<<"[NWMCL] UDP connection has been disconnected: "<<nwm::client_dropped_enum_to_string(reason)<<"!"<<std::endl;
#endif
			SchedulePollEvent([this,reason]() -> PollEventResult {
				Disconnect(reason);
				return PollEventResult::Complete;
			});
		});
		udp->SetCloseHandle([this]() {
#ifdef NWM_VERBOSE
			std::cout<<"[NWMCL] UDP connection has been closed!"<<std::endl;
#endif
			auto *udp = GetUDPConnection();
			auto err = udp->GetLastError();
			if(*err)
				m_lastError = err;
			if(GetTCPConnection() == nullptr)
			{
				SchedulePollEvent([this]() -> PollEventResult {
					OnClosed();
					return PollEventResult::Complete;
				});
			}
			else
				CloseTCPConnection();
			CloseUDPConnection();
		});
		udp->SetErrorHandle([this](const nwm::ErrorCode &errCode) {
			HandleError(nwm::Error(errCode->value(),errCode->message()));
		});
		udp->Connect(serverIp,serverPort);
	}
	if(tcp != nullptr)
	{
#ifdef NWM_VERBOSE
		std::cout<<"[NWMCL] Initializing TCP connection..."<<std::endl;
#endif
		tcp->SetPacketHandle(packetHandle);
		tcp->SetConnectionHandle([this]() {
#ifdef NWM_VERBOSE
			std::cout<<"[NWMCL] TCP connection has been established!"<<std::endl;
#endif
			SchedulePollEvent([this]() -> PollEventResult {
				OnConnected();
				return PollEventResult::Complete;
			});
		});
		tcp->SetDisconnectionHandle([this](nwm::ClientDropped reason) {
#ifdef NWM_VERBOSE
			std::cout<<"[NWMCL] TCP connection has been disconnected: "<<nwm::client_dropped_enum_to_string(reason)<<"!"<<std::endl;
#endif
			SchedulePollEvent([this,reason]() -> PollEventResult {
				Disconnect(reason);
				return PollEventResult::Complete;
			});
		});
		tcp->SetCloseHandle([this]() {
#ifdef NWM_VERBOSE
			std::cout<<"[NWMCL] TCP connection has been closed!"<<std::endl;
#endif
			auto *tcp = GetTCPConnection();
			auto err = tcp->GetLastError();
			if(*err)
				m_lastError = err;
			if(GetUDPConnection() == nullptr)
			{
				SchedulePollEvent([this]() -> PollEventResult {
					OnClosed();
					return PollEventResult::Complete;
				});
			}
			else
				CloseUDPConnection();
			CloseTCPConnection();
		});
		tcp->SetConnectCallback([this]() {
			auto *tcp = GetTCPConnection();
			m_ip = tcp->GetIP();
		});
		tcp->SetErrorHandle([this](const nwm::ErrorCode &errCode) {
			HandleError(nwm::Error(errCode->value(),errCode->message()));
		});
		tcp->Connect(serverIp,serverPort);
	}
	InitializeConnectionData();
	if(tcp != nullptr)
		m_remoteEndpoint = *tcp->GetRemoteEndPoint();
	else if(udp != nullptr)
	{
		m_ip = udp->GetIP();
		m_remoteEndpoint = *udp->GetRemoteEndPoint();
	}
#ifdef NWM_VERBOSE
	std::cout<<"[NWMCL] Initialization complete!"<<std::endl;
#endif
}
void nwm::Client::OnPacketReceived(const NWMEndpoint &ep,unsigned int id,NetPacket &packet) {}
void nwm::Client::SendPacket(nwm::Protocol protocol,const NetPacket &packet) {SendPacket(protocol,packet,false);}
bool nwm::Client::HandlePacket(const NWMEndpoint &ep,uint32_t id,NetPacket &packet)
{
#ifdef NWM_VERBOSE
	std::cout<<"[NWMCL] Handling incoming packet "<<id<<std::endl;
#endif
	OnPacketReceived(ep,id,packet);
	if(id == NWM_MESSAGE_IN_REGISTER_CONFIRMATION)
	{
		OnConnected();
		return true;
	}
	else if(id == NWM_MESSAGE_IN_DROPPED)
	{
		auto reason = packet->Read<int8_t>();
		Disconnect(static_cast<ClientDropped>(reason));
		return true;
	}
	else if(id == NWM_MESSAGE_IN_PONG)
	{
		Pong();
		return true;
	}
	return false;
}
void nwm::Client::Disconnect(ClientDropped reason)
{
#ifdef NWM_VERBOSE
	std::cout<<"[NWMCL] Disconnecting with reason '"<<nwm::client_dropped_enum_to_string(reason)<<"'..."<<std::endl;
#endif
	OnDisconnected(reason);
	Close();
}
void nwm::Client::SendPacket(nwm::Protocol protocol,const NetPacket &packet,bool bOwn)
{
	if(IsActive() == false)
		return;
#ifdef NWM_VERBOSE
	std::cout<<"[NWMCL] Sending packet "<<packet.GetMessageID()<<" using protocol "<<nwm::protocol_enum_to_string(protocol)<<"..."<<std::endl;
#endif
	OnPacketSent(GetRemoteEndpoint(),packet);
	if(HasUDPConnection() == false)
		protocol = nwm::Protocol::TCP;
	auto ep = GetRemoteEndpoint();
	impl::ManagerBase::SendPacket(protocol,packet,ep,bOwn);
}
void nwm::Client::Ping()
{
	m_lastPingMutex.lock();
		m_tLastPing = util::Clock::now();
	m_lastPingMutex.unlock();
	NetPacket p(NWM_MESSAGE_OUT_PING);
	p->Write<uint16_t>(m_latency);
	SendPacket(Protocol::UDP,p);
}
void nwm::Client::Pong()
{
	const auto ratio = 0.8L;
	for(auto i=decltype(m_latencies.size()){4};i>0;--i)
		m_latencies.at(i -1) = m_latencies.at(i);

	m_lastPingMutex.lock();
		auto lastPing = m_tLastPing;
	m_lastPingMutex.unlock();
	m_latencies.back() = static_cast<uint16_t>(
		round(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				util::Clock::now() -lastPing
			).count() *ratio +m_latency *(1.0 -ratio)
		)
	);
	uint16_t sum = 0;
	for(auto l : m_latencies)
		sum += l;
	m_latency = static_cast<uint16_t>(round(sum /m_latencies.size()));
}
void nwm::Client::Run()
{
	if(IsClosing())
		return;
	if(m_bPingEnabled == false)
		return;
	auto now = util::Clock::now();
	m_lastPingMutex.lock();
		auto lastPing = m_tLastPing;
	m_lastPingMutex.unlock();
	if(std::chrono::duration_cast<std::chrono::seconds>(now -lastPing).count() < PING_INTERVAL)
		return;
	Ping();
}
void nwm::Client::Disconnect()
{
	if(IsClosing())
		return;
#ifdef NWM_VERBOSE
		std::cout<<"[NWMCL] Disconnecting from server..."<<std::endl;
#endif
	ScheduleAsyncEvent([this]() {
		auto *conUdp = GetUDPConnection();
		if(conUdp != nullptr)
			static_cast<CLNWMUDPConnection*>(conUdp)->Disconnect();
		auto *conTcp = GetTCPConnection();
		if(conTcp != nullptr)
			static_cast<CLNWMTCPConnection*>(conTcp)->Disconnect();
		CloseAsync();
	});
}
uint16_t nwm::Client::GetLatency() const {return m_latency;}
const std::string &nwm::Client::GetIP() const {return m_ip;}
const nwm::ErrorCode &nwm::Client::GetLastError() const {return m_lastError;}
const NWMEndpoint &nwm::Client::GetRemoteEndpoint() const {return m_remoteEndpoint;}
void nwm::Client::SetPingEnabled(bool b) {m_bPingEnabled = b;}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
