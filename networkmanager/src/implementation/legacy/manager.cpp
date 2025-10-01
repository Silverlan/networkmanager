// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <memory>
#include <functional>

module pragma.network_manager;

import :legacy.manager;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMError::NWMError(int err, const std::string &msg) : m_value(err), m_message(msg) {}
const char *NWMError::name() const { return "nwm"; }
std::string NWMError::message() const { return m_message; }
int NWMError::value() const { return m_value; }

//////////////////////////////////////////

std::string GetClientDroppedReasonString(CLIENT_DROPPED reason)
{
	switch(reason) {
	case CLIENT_DROPPED::DISCONNECTED:
		return "disconnected";
	case CLIENT_DROPPED::TIMEOUT:
		return "timed out";
	case CLIENT_DROPPED::KICKED:
		return "kicked";
	case CLIENT_DROPPED::SHUTDOWN:
		return "server shut down";
	case CLIENT_DROPPED::ERROR:
		return "error";
	}
	return "unknown";
}

//////////////////////////////////////////

NWManagerBase::NWManagerBase(const std::shared_ptr<NWMUDPConnection> &udp, const std::shared_ptr<NWMTCPConnection> &tcp) : m_conUDP(udp), m_conTCP(tcp), m_thread(nullptr), m_bClosing(false)
{
	if(udp != nullptr) {
		udp->SetCloseHandle([this]() { m_conUDP = nullptr; });
	}
	if(tcp != nullptr) {
		tcp->SetCloseHandle([this]() { m_conTCP = nullptr; });
	}
}

NWManagerBase::~NWManagerBase() { m_thread = nullptr; }

void NWManagerBase::QueueEvent(const std::function<void(void)> &f)
{
	m_pollQueueMutex.lock();
	m_pollQueue.push(f);
	m_pollQueueMutex.unlock();
}

void NWManagerBase::QueueAsyncEvent(const std::function<void(void)> &f)
{
	if(IsAsync() == false) {
		f();
		return;
	}
	m_asyncPollQueueMutex.lock();
	m_asyncPollQueue.push(f);
	m_asyncPollQueueMutex.unlock();
}

void NWManagerBase::PollAsyncEvents()
{
	m_asyncPollQueueMutex.lock();
	while(m_asyncPollQueue.empty() == false) {
		auto &f = m_asyncPollQueue.front();
		f();
		m_asyncPollQueue.pop();
	}
	m_asyncPollQueueMutex.unlock();
}

void NWManagerBase::Release()
{
	if(m_conUDP != nullptr)
		m_conUDP->Close();
	if(m_conTCP != nullptr)
		m_conTCP->Close();
	if(m_thread != nullptr)
		m_thread->join();
}

void NWManagerBase::SetNagleAlgorithmEnabled(bool b)
{
	if(m_conTCP == nullptr)
		return;
	m_conTCP->SetNagleAlgorithmEnabled(b);
}

void NWManagerBase::SetTimeoutDuration(double duration)
{
	if(m_conUDP != nullptr) {
		m_conUDP->SetTimeoutDuration(duration);
		if(m_conTCP != nullptr)
			m_conTCP->SetTimeoutDuration(0.0);
		return;
	}
	if(m_conTCP != nullptr)
		m_conTCP->SetTimeoutDuration(duration);
}

NWMUDPConnection *NWManagerBase::GetUDPConnection() const { return m_conUDP.get(); }
NWMTCPConnection *NWManagerBase::GetTCPConnection() const { return m_conTCP.get(); }

void NWManagerBase::CloseConnections()
{
	if(m_conTCP != nullptr && !m_conTCP->IsClosing())
		m_conTCP->Close();
	if(m_conUDP != nullptr && !m_conUDP->IsClosing())
		m_conUDP->Close();
}

void NWManagerBase::Run()
{
	PollAsyncEvents();
	if(m_bClosing == true)
		CloseConnections();
	SendPackets();
	if(m_conTCP != nullptr)
		m_conTCP->Run();
	if(m_conUDP != nullptr)
		m_conUDP->Run();
}

void NWManagerBase::Poll()
{
	if(m_conTCP != nullptr)
		m_conTCP->Poll();
	if(m_conUDP != nullptr)
		m_conUDP->Poll();
}

void NWManagerBase::PollEvents()
{
	m_pollQueueMutex.lock();
	while(m_pollQueue.empty() == false) {
		auto &f = m_pollQueue.front();
		f();
		m_pollQueue.pop();
	}
	m_pollQueueMutex.unlock();
}

bool NWManagerBase::IsActive() const
{
	if(m_conTCP == nullptr) {
		if(m_conUDP == nullptr)
			return false;
		return m_conUDP->IsActive();
	}
	return (m_conTCP->IsActive() || (m_conUDP != nullptr && m_conUDP->IsActive())) ? true : false;
}

bool NWManagerBase::IsClosing() const
{
	if(m_bClosing == true)
		return m_bClosing;
	if(m_conTCP != nullptr) {
		if(m_conTCP->IsClosing())
			return true;
	}
	if(m_conUDP != nullptr) {
		if(m_conUDP->IsClosing())
			return true;
	}
	return false;
}

void NWManagerBase::Close() { m_bClosing = true; }

void NWManagerBase::HandleError(const NWMError &err)
{
	if(m_cbError != nullptr)
		m_cbError(err);
}

bool NWManagerBase::HasUDPConnection() const { return (m_conUDP != nullptr) ? true : false; }
bool NWManagerBase::HasTCPConnection() const { return (m_conTCP != nullptr) ? true : false; }
void NWManagerBase::SetErrorHandle(const std::function<void(const NWMError &)> &cbError) { m_cbError = cbError; }

void NWManagerBase::OnPacketSent(const NWMEndpoint &ep, const NetPacket &packet) {}

void NWManagerBase::SendPacket(SendPacketQueue &item)
{
	OnPacketSent(item.ep, item.packet);
	if(item.tcp == true) {
		if(m_conTCP == nullptr)
			return;
		m_conTCP->SendPacket(item.packet, item.ep, item.own);
		return;
	}
	if(m_conUDP == nullptr)
		return;
	m_conUDP->SendPacket(item.packet, item.ep, item.own);
}

void NWManagerBase::SendPackets()
{
	m_sendPacketMutex.lock();
	while(!m_sendPacketQueue.empty()) {
		SendPacketQueue &item = m_sendPacketQueue.front();
		SendPacket(item);
		m_sendPacketQueue.pop();
	}
	m_sendPacketMutex.unlock();
}

void NWManagerBase::SendPacketTCP(const NetPacket &packet, NWMTCPEndpoint &ep, bool bOwn)
{
	SendPacketQueue item(packet, ep, bOwn, true);
	m_sendPacketMutex.lock();
	m_sendPacketQueue.push(item);
	m_sendPacketMutex.unlock();
}
void NWManagerBase::SendPacketUDP(const NetPacket &packet, NWMUDPEndpoint &ep, bool bOwn)
{
	SendPacketQueue item(packet, ep, bOwn, false);
	m_sendPacketMutex.lock();
	m_sendPacketQueue.push(item);
	m_sendPacketMutex.unlock();
}

void NWManagerBase::SendPacket(const NetPacket &packet, NWMEndpoint &ep, bool bPreferUDP)
{
	if(bPreferUDP == true && m_conUDP != nullptr) {
		SendPacketUDP(packet, *static_cast<NWMUDPEndpoint *>(ep.get()));
		return;
	}
	if(m_conTCP != nullptr) {
		SendPacketTCP(packet, *static_cast<NWMTCPEndpoint *>(ep.get()));
		return;
	}
	if(m_conUDP == nullptr)
		return;
	SendPacketUDP(packet, *static_cast<NWMUDPEndpoint *>(ep.get()));
}

void NWManagerBase::Start()
{
	while(IsActive())
		Run();
}

void NWManagerBase::StartAsync()
{
	if(m_thread != nullptr)
		return;
	m_thread = std::make_unique<std::thread>(std::bind(&NWManagerBase::Start, this));
}

bool NWManagerBase::IsAsync() const { return (m_thread != nullptr) ? true : false; }

std::string NWManagerBase::GetLocalIP() const
{
	if(m_conTCP == nullptr) {
		if(m_conUDP == nullptr)
			return nwm::invalid_address();
		return m_conUDP->GetLocalIP();
	}
	return m_conTCP->GetLocalIP();
}
unsigned short NWManagerBase::GetLocalPort() const
{
	if(m_conTCP == nullptr) {
		if(m_conUDP == nullptr)
			return 0;
		return m_conUDP->GetLocalPort();
	}
	return m_conTCP->GetLocalPort();
}
unsigned short NWManagerBase::GetLocalUDPPort() const
{
	if(m_conUDP == nullptr)
		return 0;
	return m_conUDP->GetLocalPort();
}
unsigned short NWManagerBase::GetLocalTCPPort() const
{
	if(m_conTCP == nullptr)
		return 0;
	return m_conTCP->GetLocalPort();
}
nwm::IPAddress NWManagerBase::GetLocalAddress() const
{
	if(m_conTCP == nullptr) {
		if(m_conUDP == nullptr)
			return nwm::IPAddress {};
		return m_conUDP->GetLocalAddress();
	}
	return m_conTCP->GetLocalAddress();
}

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
