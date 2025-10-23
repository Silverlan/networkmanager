// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>
#include <memory>
#include <chrono>

module pragma.client_manager;

import :enums;
import :legacy.manager;

#pragma comment(lib, "networkmanager.lib")
/*
NWMClient *NWMClient::Create(const std::string &serverIp,unsigned int serverPort,unsigned int minClientPort,unsigned int maxClientPort,unsigned char conType)
{
	return Create<NWMClient>(serverIp,serverPort,minClientPort,maxClientPort,conType);
}
*/
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMClient::NWMClient(const std::shared_ptr<CLNWMUDPConnection> &udp, const std::shared_ptr<CLNWMTCPConnection> &tcp) : NWManagerBase(udp, tcp), m_latency(0), m_lastError {}, m_bPingEnabled(true) {}

NWMClient::~NWMClient() {}

void NWMClient::SetPingEnabled(bool b) { m_bPingEnabled = b; }

void NWMClient::Initialize(const std::string &serverIp, unsigned short serverPort)
{
	auto udp = std::static_pointer_cast<CLNWMUDPConnection>(m_conUDP);
	auto tcp = std::static_pointer_cast<CLNWMTCPConnection>(m_conTCP);
	for(char i = 0; i < 5; i++)
		m_latencies[i] = 0;
	auto packetHandle = std::bind(
	  [](NWMClient *cl, const NWMEndpoint &, NWMIOBase *io, unsigned int id, NetPacket &readPacket) {
		  if(cl->HandlePacket(dynamic_cast<NWMIO *>(io), id, readPacket) == false)
			  cl->HandleError(NWMError(NETWORK_ERROR_UNHANDLED_PACKET, "Unhandled packet '" + std::to_string(id) + '\''));
	  },
	  this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	if(udp != nullptr) {
		udp->SetPacketHandle(packetHandle);
		udp->SetConnectionHandle([this]() { QueueEvent(std::bind(&NWMClient::OnConnected, this)); });
		udp->SetDisconnectionHandle([this](nwm::ClientDropped reason) { QueueEvent(std::bind(&NWMClient::OnDisconnected, this, static_cast<CLIENT_DROPPED>(reason))); });
		udp->SetCloseHandle([this]() {
			auto err = static_cast<CLNWMUDPConnection *>(this->m_conUDP.get())->GetLastError();
			if(*err)
				m_lastError = err;
			auto conUdp = this->m_conUDP;
			auto conTcp = this->m_conTCP;
			if(m_conTCP == nullptr) {
				QueueEvent([this]() { OnClosed(); });
			}
			else
				m_conTCP->Close();
			m_conUDP = nullptr;
		});
		udp->Connect(serverIp, serverPort);
	}
	if(tcp != nullptr) {
		tcp->SetPacketHandle(packetHandle);
		tcp->SetConnectionHandle([this]() { QueueEvent(std::bind(&NWMClient::OnConnected, this)); });
		tcp->SetDisconnectionHandle([this](nwm::ClientDropped reason) { QueueEvent(std::bind(&NWMClient::OnDisconnected, this, static_cast<CLIENT_DROPPED>(reason))); });
		tcp->SetCloseHandle([this]() {
			auto err = static_cast<CLNWMTCPConnection *>(this->m_conTCP.get())->GetLastError();
			if(*err)
				m_lastError = err;
			auto conUdp = this->m_conUDP;
			auto conTcp = this->m_conTCP;
			if(m_conUDP == nullptr) {
				QueueEvent([this]() { OnClosed(); });
			}
			else
				m_conUDP->Close();
			m_conTCP = nullptr;
		});
		tcp->Connect(serverIp, serverPort);
	}
}

unsigned short NWMClient::GetLatency() const { return m_latency; }
const nwm::ErrorCode &NWMClient::GetLastError() const { return m_lastError; }

std::string NWMClient::GetIP()
{
	if(m_conTCP != nullptr)
		return std::static_pointer_cast<CLNWMTCPConnection>(m_conTCP)->GetIP();
	if(m_conUDP == nullptr)
		return "";
	return std::static_pointer_cast<CLNWMUDPConnection>(m_conUDP)->GetIP();
}

void NWMClient::Run()
{
	NWManagerBase::Run();
	if(IsClosing())
		return;
	if(m_bPingEnabled == false)
		return;
	auto now = util::Clock::now();
	m_lastPingMutex.lock();
	auto lastPing = m_tLastPing;
	m_lastPingMutex.unlock();
	if(std::chrono::duration_cast<std::chrono::seconds>(now - lastPing).count() < NWM_CLIENT_PING_INTERVAL)
		return;
	Ping();
}

void NWMClient::Ping()
{
	m_lastPingMutex.lock();
	m_tLastPing = util::Clock::now();
	m_lastPingMutex.unlock();
	NetPacket p(NWM_MESSAGE_OUT_PING);
	p->Write<unsigned short>(m_latency);
	SendPacketUDP(p);
}

void NWMClient::Pong()
{
	const long double ratio = 0.8;
	for(char i = 4; i > 0; i--)
		m_latencies[i - 1] = m_latencies[i];
	m_lastPingMutex.lock();
	ChronoTimePoint lastPing = m_tLastPing;
	m_lastPingMutex.unlock();
	m_latencies[4] = static_cast<unsigned short>(round(std::chrono::duration_cast<std::chrono::milliseconds>(util::Clock::now() - lastPing).count() * ratio + m_latency * (1.0 - ratio)));
	unsigned short sum = 0;
	for(char i = 0; i < 5; i++)
		sum += m_latencies[i];
	m_latency = static_cast<unsigned short>(round(sum / 5));
}

void NWMClient::SendPacketTCP(const NetPacket &packet) { SendPacketTCP(packet, false); }
void NWMClient::SendPacketUDP(const NetPacket &packet) { SendPacketUDP(packet, false); }

void NWMClient::SendPacketTCP(const NetPacket &packet, bool bOwn)
{
	if(m_conTCP == nullptr) {
		if(m_conUDP != nullptr)
			return SendPacketUDP(packet, bOwn);
	}
	NWMTCPEndpoint *ep = (m_conTCP != nullptr) ? std::static_pointer_cast<CLNWMTCPConnection>(m_conTCP)->GetRemoteEndPoint() : nullptr;
	if(ep != nullptr)
		NWManagerBase::SendPacketTCP(packet, *ep, bOwn);
	else {
		NWMTCPEndpoint ep(nullptr);
		NWManagerBase::SendPacketTCP(packet, ep, bOwn);
	}
}
void NWMClient::SendPacketUDP(const NetPacket &packet, bool bOwn)
{
	if(m_conUDP == nullptr) {
		if(m_conTCP != nullptr)
			return SendPacketTCP(packet, bOwn);
	}
	NWMUDPEndpoint *ep = (m_conUDP != nullptr) ? std::static_pointer_cast<CLNWMUDPConnection>(m_conUDP)->GetRemoteEndPoint() : nullptr;
	if(ep != nullptr)
		NWManagerBase::SendPacketUDP(packet, *ep, bOwn);
	else {
		NWMUDPEndpoint ep(nullptr);
		NWManagerBase::SendPacketUDP(packet, ep, bOwn);
	}
}

void NWMClient::Close()
{
	if(IsClosing())
		return;
	NWManagerBase::Close();
	if(m_conUDP != nullptr)
		m_conUDP->Close();
	if(m_conTCP != nullptr)
		m_conTCP->Close();
}
void NWMClient::Disconnect()
{
	if(IsClosing())
		return;
	QueueAsyncEvent([this]() {
		if(m_conUDP != nullptr)
			std::static_pointer_cast<CLNWMUDPConnection>(m_conUDP)->Disconnect();
		else if(m_conTCP != nullptr)
			std::static_pointer_cast<CLNWMTCPConnection>(m_conTCP)->Disconnect();
		Close();
	}); // If not async, executed immediately
	if(IsAsync())
		return;
	while(IsActive())
		Run();
	/*while(IsActive())
	{
		if(!IsAsync())
			Run();
	}*/
}

void NWMClient::CloseConnections()
{
	if(m_conTCP != nullptr && !m_conTCP->IsClosing())
		std::static_pointer_cast<CLNWMTCPConnection>(m_conTCP)->Close();
	if(m_conUDP != nullptr && !m_conUDP->IsClosing())
		std::static_pointer_cast<CLNWMUDPConnection>(m_conUDP)->Close();
}

//////////////////////////////////////////

NWMServerInfo::NWMServerInfo() : m_numClients(0), m_maxClients(0) {}
unsigned int NWMServerInfo::GetClientCount() const { return m_numClients; }
unsigned int NWMServerInfo::GetMaxClients() const { return m_maxClients; }
void NWMServerInfo::Read(NetPacket &packet)
{
	m_numClients = packet->Read<unsigned int>();
	m_maxClients = packet->Read<unsigned int>();
}
void NWMServerInfo::Write(NetPacket &packet)
{
	packet->Write<unsigned int>(GetClientCount());
	packet->Write<unsigned int>(GetMaxClients());
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
