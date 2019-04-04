/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/nwm_boost.h"
#include "networkmanager/interface/nwm_manager.hpp"
#include "networkmanager/wrappers/nwm_boost_wrapper_impl.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
std::string nwm::client_dropped_enum_to_string(ClientDropped e)
{
	switch(e)
	{
	case ClientDropped::Disconnected:
		return "disconnected";
	case ClientDropped::Timeout:
		return "timed out";
	case ClientDropped::Kicked:
		return "kicked";
	case ClientDropped::Shutdown:
		return "server shut down";
	case ClientDropped::Error:
		return "error";
	}
	return "unknown";
}

std::string nwm::protocol_enum_to_string(Protocol e)
{
	switch(e)
	{
		case Protocol::TCP:
			return "TCP";
		case Protocol::UDP:
			return "UDP";
	}
	return "unknown";
}

nwm::Error::Error(int err,const std::string &msg)
	: m_value(err),m_message(msg)
{}
const char *nwm::Error::name() const {return "nwm";}
std::string nwm::Error::message() const {return m_message;}
int32_t nwm::Error::value() const {return m_value;}

//////////////////////////////////////

nwm::impl::ManagerBase::ManagerBase(const std::shared_ptr<NWMUDPConnection> &udp,const std::shared_ptr<NWMTCPConnection> &tcp)
	: m_conUDP(udp),m_conTCP(tcp)
{}

nwm::impl::ManagerBase::~ManagerBase()
{
	if(m_thread != nullptr && m_thread->joinable())
		m_thread->join();
	m_thread = nullptr;
}

void nwm::impl::ManagerBase::InitializeConnectionData()
{
#ifdef NWM_VERBOSE
		std::cout<<"[NWM] Initializing connection data..."<<std::endl;
#endif
	auto &udp = m_conUDP;
	auto &tcp = m_conTCP;
	if(tcp != nullptr)
	{
		m_bHasTCP = true;
		m_localIp = tcp->GetLocalIP();
		m_localTCPPort = tcp->GetLocalPort();
		tcp->SetCloseHandle([this]() {
			m_conTCP = nullptr;
		});
	}
	else if(udp != nullptr)
		m_localIp = udp->GetLocalIP();
	else
		m_localIp = nwm::invalid_address();

	if(udp != nullptr)
	{
		m_bHasUDP = true;
		m_localUDPPort = udp->GetLocalPort();
		udp->SetCloseHandle([this]() {
			m_conUDP = nullptr;
		});
	}
}

void nwm::impl::ManagerBase::OnPacketSent(const NWMEndpoint &ep,const NetPacket &packet) {}

void nwm::impl::ManagerBase::SetErrorHandle(const std::function<void(const Error&)> &cbError) {m_cbError = cbError;}
bool nwm::impl::ManagerBase::IsActive() const {return m_bActive;}
bool nwm::impl::ManagerBase::IsClosing() const {return !IsActive();}

NWMUDPConnection *nwm::impl::ManagerBase::GetUDPConnection() {return m_conUDP.get();}
NWMTCPConnection *nwm::impl::ManagerBase::GetTCPConnection() {return m_conTCP.get();}
void nwm::impl::ManagerBase::CloseUDPConnection()
{
	if(m_conUDP == nullptr)
		return;
	m_conUDP->Close();
	m_conUDP = nullptr;
}
void nwm::impl::ManagerBase::CloseTCPConnection()
{
	if(m_conTCP == nullptr)
		return;
	m_conTCP->Close();
	m_conTCP = nullptr;
}

void nwm::impl::ManagerBase::HandleError(const Error &err)
{
	if(m_cbError == nullptr)
		return;
	m_cbError(err);
}

void nwm::impl::ManagerBase::Start()
{
	if(m_thread != nullptr)
		return;
	m_bActive = true;
	m_thread = std::make_unique<std::thread>([this]() {
		while(IsActive())
		{
			Run();
			if(IsActive() == false)
				break;
			if(m_conTCP != nullptr)
				m_conTCP->Run();
			if(m_conUDP != nullptr)
				m_conUDP->Run();
			if(m_bHasThreadEvents == true)
			{
				m_eventMutex.lock();
				m_threadEventMutex.lock();
					while(m_threadEventQueue.empty() == false)
					{
						auto &f = m_threadEventQueue.front();
						f();
						m_threadEventQueue.pop();
					}
					m_bHasThreadEvents = false;
				m_threadEventMutex.unlock();
				m_eventMutex.unlock();
			}
		}
		if(m_conTCP != nullptr)
		{
			if(m_conTCP->IsClosing() == false)
				m_conTCP->Close();
			while(m_conTCP != nullptr && m_conTCP->IsClosing())
				m_conTCP->Run();
		}
		if(m_conUDP != nullptr)
		{
			if(m_conUDP->IsClosing() == false)
				m_conUDP->Close();
			while(m_conUDP != nullptr && m_conUDP->IsClosing())
				m_conUDP->Run();
		}
	});
}

void nwm::impl::ManagerBase::ScheduleAsyncEvent(const std::function<void(void)> &f)
{
	m_threadEventMutex.lock();
		m_threadEventQueue.push(f);
	m_threadEventMutex.unlock();
	m_bHasThreadEvents = true;
}
void nwm::impl::ManagerBase::SchedulePollEvent(const std::function<PollEventResult(void)> &f)
{
	m_pollEventMutex.lock();
		m_pollEventQueue.push_back(f);
	m_pollEventMutex.unlock();
	m_bHasPollEvents = true;
}
void nwm::impl::ManagerBase::PollEvents()
{
	if(m_bHasPollEvents == false)
		return;
	m_eventMutex.lock();
	m_pollEventMutex.lock();
		// No iterators, because m_pollEventQueue can potentially change during iteration
		// (It can only get larger, not smaller, so iterating up to 'numEvents' is fine)
		for(auto i=decltype(m_pollEventQueue.size()){0};i<m_pollEventQueue.size();)
		{
			auto &f = m_pollEventQueue.at(i);
			if(f() == PollEventResult::Complete)
				m_pollEventQueue.erase(m_pollEventQueue.begin() +i);
			else
				++i;
		}
		m_bHasPollEvents = !m_pollEventQueue.empty();
	m_pollEventMutex.unlock();
	m_eventMutex.unlock();
}
void nwm::impl::ManagerBase::SendPacket(Protocol protocol,const NetPacket &packet,NWMEndpoint &ep,bool bOwn)
{
	ScheduleAsyncEvent([this,protocol,packet,ep,bOwn]() {
		switch(protocol)
		{
			case Protocol::TCP:
			{
				if(m_conTCP == nullptr)
					return;
				m_conTCP->SendPacket(packet,ep,bOwn);
				break;
			}
			case Protocol::UDP:
			{
				if(m_conUDP == nullptr)
					return;
				m_conUDP->SendPacket(packet,ep,bOwn);
				break;
			}
		}
	});
}

void nwm::impl::ManagerBase::Close()
{
	if(m_bActive == false)
		return;
	ScheduleAsyncEvent([this]() {
		CloseAsync();
	});
	m_bActive = false;
}

bool nwm::impl::ManagerBase::HasUDPConnection() const {return m_bHasUDP;}
bool nwm::impl::ManagerBase::HasTCPConnection() const {return m_bHasTCP;}
void nwm::impl::ManagerBase::SetTimeoutDuration(double duration)
{
	ScheduleAsyncEvent([this,duration]() {
		if(m_conUDP != nullptr)
		{
			m_conUDP->SetTimeoutDuration(duration);
			if(m_conTCP != nullptr)
				m_conTCP->SetTimeoutDuration(0.0);
			return;
		}
		if(m_conTCP != nullptr)
			m_conTCP->SetTimeoutDuration(duration);
	});
}
std::string nwm::impl::ManagerBase::GetLocalIP() const {return m_localIp;}
uint16_t nwm::impl::ManagerBase::GetLocalPort() const {return (m_localTCPPort != 0) ? m_localTCPPort : GetLocalUDPPort();}
uint16_t nwm::impl::ManagerBase::GetLocalUDPPort() const {return m_localUDPPort;}
uint16_t nwm::impl::ManagerBase::GetLocalTCPPort() const {return m_localTCPPort;}
nwm::IPAddress nwm::impl::ManagerBase::GetLocalAddress() const {return m_localAddress;}
void nwm::impl::ManagerBase::SetNagleAlgorithmEnabled(bool b)
{
	if(HasTCPConnection() == false)
		return;
	ScheduleAsyncEvent([this,b]() {
		if(m_conTCP == nullptr)
			return;
		m_conTCP->SetNagleAlgorithmEnabled(b);
	});
}

// NOT thread-safe!
void nwm::impl::ManagerBase::CloseAsync()
{
#ifdef NWM_VERBOSE
		std::cout<<"[NWM] Closing connection..."<<std::endl;
#endif
	if(m_bActive == false)
		return;
	if(m_conTCP != nullptr)
		m_conTCP->Close();
	if(m_conUDP != nullptr)
		m_conUDP->Close();
	m_bActive = false;
}

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
