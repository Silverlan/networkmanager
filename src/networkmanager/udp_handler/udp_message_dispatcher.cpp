/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/nwm_boost.h"
#include "networkmanager/udp_handler/udp_message_dispatcher.h"
#include "networkmanager/wrappers/nwm_impl_boost.hpp"
#include "networkmanager/wrappers/nwm_boost_wrapper_impl.hpp"
#include <boost/bind.hpp>

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
UDPMessageDispatcher::UDPMessageDispatcher() : UDPMessageBase(), m_resolver(m_ioService), m_active(0), m_deadline(m_ioService), m_timeout(0)
{
	cast_deadline_timer(m_deadline)->expires_at(boost::posix_time::pos_infin);
	CheckDeadline();
}

UDPMessageDispatcher::~UDPMessageDispatcher()
{
	while(IsActive())
		Poll();
}

std::unique_ptr<UDPMessageDispatcher> UDPMessageDispatcher::Create(unsigned int timeout)
{
	auto r = std::unique_ptr<UDPMessageDispatcher>(new UDPMessageDispatcher());
	r->SetTimeout(timeout);
	return r;
}

void UDPMessageDispatcher::CheckDeadline()
{
	if(cast_deadline_timer(m_deadline)->expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
		Cancel();
		cast_deadline_timer(m_deadline)->expires_at(boost::posix_time::pos_infin);
	}
	cast_deadline_timer(m_deadline)->async_wait(boost::bind(&UDPMessageDispatcher::CheckDeadline, this));
}

void UDPMessageDispatcher::Cancel()
{
	m_messageMutex.lock();
	for(auto it = m_messages.begin(); it != m_messages.end(); ++it) // Cancel all current io operations
	{
		auto &msg = *it;
		msg->Cancel();
	}
	m_messageMutex.unlock();
}

bool UDPMessageDispatcher::IsActive() const { return (m_active != 0) ? true : false; }

void UDPMessageDispatcher::Update()
{
	if(m_active == 0)
		return;
	m_eventMutex.lock();
	while(!m_events.empty()) {
		auto &f = m_events.front();
		f();
		m_events.pop();
		m_active--;
	}
	m_eventMutex.unlock();
	m_messageMutex.lock();
	for(size_t i = 0; i < m_messages.size();) {
		m_messages[i]->Poll(); // This can potentially increase (but not decrease!) the size of 'm_messages'
		auto &msg = m_messages[i];
		msg->Poll();
		if(!msg->IsActive()) {
			m_messages.erase(m_messages.begin() + i);
			m_active--;
		}
		else
			++i;
	}
	m_messageMutex.unlock();
}

void UDPMessageDispatcher::Poll()
{
	if(m_active == 0)
		return;
	Update();
	UDPMessageBase::Poll();
}

void UDPMessageDispatcher::ResetTimeout()
{
	if(m_timeout == 0) {
		cast_deadline_timer(m_deadline)->expires_at(boost::posix_time::pos_infin);
		return;
	}
	cast_deadline_timer(m_deadline)->expires_from_now(boost::posix_time::seconds(m_timeout));
}

void UDPMessageDispatcher::ScheduleMessage(DispatchInfo &info, const nwm::UDPEndpoint &ep)
{
	m_messageMutex.lock();
	m_active++;
	m_messages.push_back(std::unique_ptr<Message>((info.socket == nullptr) ? (new Message(m_ioService)) : (new Message(*info.socket))));
	auto &msg = m_messages.back();
	msg->data = info.data;
	msg->callback = info.callback;
	msg->endpoint = ep;
	msg->Dispatch();
	m_messageMutex.unlock();
}

void UDPMessageDispatcher::ResolveNext(bool lockMutex)
{
	ResetTimeout();
	if(lockMutex == true)
		m_dispatchQueueMutex.lock();
	auto &msg = m_dispatchQueue.front();
	if(msg.valid_endpoint == false) {
		m_resolverQuery = std::make_unique<nwm::UDPResolverQuery>(msg.ip, std::to_string(msg.port));
		nwm::cast_resolver(m_resolver)
		  ->async_resolve(
#if NWM_USE_IPV6 == 0
		    udp::v4(),
		    "0.0.0.0", // IPv4 wildcard
#else
		    udp::v6(),
		    "::1", // IPv6 loopback
#endif
		    std::to_string(msg.port), // port
		    [this](const boost::system::error_code &ec, udp::resolver::results_type results) {
			    m_dispatchQueueMutex.lock();
			    auto &info = m_dispatchQueue.front();
			    if(ec) {
				    auto &callback = info.callback;
				    m_eventMutex.lock();
				    m_active++;
				    m_events.push([callback, ec]() { callback(ec, nullptr); });
				    m_eventMutex.unlock();
			    }
			    else {
				    boost::asio::ip::udp::endpoint ep = *results.begin();
				    ScheduleMessage(info, nwm::UDPEndpoint {&ep});
			    }
			    m_active--;
			    m_dispatchQueue.pop();
			    if(m_dispatchQueue.empty()) {
				    m_dispatchQueueMutex.unlock();
				    return;
			    }
			    // m_dispatchQueueMutex.unlock(); // It'll be unlocked through 'ResolveNext', but it has to stay locked until then
			    ResolveNext(false);
		    });
	}
	else {
		ScheduleMessage(msg, msg.endpoint);
		m_dispatchQueue.pop();
		if(!m_dispatchQueue.empty()) {
			ResolveNext(false);
			return;
		}
	}
	m_dispatchQueueMutex.unlock();
}

void UDPMessageDispatcher::Dispatch(const DispatchInfo &info)
{
	m_active++;
	m_dispatchQueueMutex.lock();
	m_dispatchQueue.push(info);
	auto bFirst = (m_dispatchQueue.size() == 1) ? true : false;
	m_dispatchQueueMutex.unlock();
	if(bFirst == true)
		ResolveNext();
}
void UDPMessageDispatcher::Dispatch(DataStream &data, const std::string &ip, unsigned short port, std::function<void(nwm::ErrorCode, Message *)> callback) { Dispatch(DispatchInfo(data, ip, port, callback)); }
void UDPMessageDispatcher::Dispatch(DataStream &data, const nwm::UDPEndpoint &ep, std::function<void(nwm::ErrorCode, Message *)> callback) { Dispatch(DispatchInfo(data, ep, callback)); }
void UDPMessageDispatcher::Dispatch(DataStream &data, const nwm::UDPEndpoint &ep, nwm::UDPSocket &socket, std::function<void(nwm::ErrorCode, Message *)> callback) { Dispatch(DispatchInfo(data, ep, socket, callback)); }

unsigned int UDPMessageDispatcher::GetTimeout() const { return m_timeout; }
void UDPMessageDispatcher::SetTimeout(unsigned int t) { m_timeout = t; }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
