/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/nwm_boost.h"
#include "networkmanager/udp_handler/udp_message_dispatcher.h"
#include "networkmanager/wrappers/nwm_impl_boost.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
UDPMessageDispatcher::Message::Message(nwm::IOService &ioService, std::function<void(nwm::ErrorCode, Message *)> _callback) : socket(std::make_unique<nwm::UDPSocket>(ioService, boost::asio::ip::udp::v4())), callback(_callback), m_bComplete(false), external_socket(nullptr) {}

UDPMessageDispatcher::Message::Message(nwm::UDPSocket &_socket, std::function<void(nwm::ErrorCode, Message *)> _callback) : external_socket(&_socket), callback(_callback), m_bComplete(false) {}

UDPMessageDispatcher::Message::~Message()
{
	if(socket != nullptr)
		nwm::cast_socket(*socket)->close();
}

bool UDPMessageDispatcher::Message::IsActive() const { return !m_bComplete; }

void UDPMessageDispatcher::Message::Poll()
{
	m_eventMutex.lock();
	while(!m_events.empty()) {
		auto &f = m_events.front();
		f();
		m_events.pop();
	}
	m_eventMutex.unlock();
}

void UDPMessageDispatcher::Message::Cancel()
{
	// socket->cancel();
	if(socket != nullptr)
		nwm::cast_socket(*socket)->close();
}

void UDPMessageDispatcher::Message::Receive(unsigned int size, std::function<void(nwm::ErrorCode, DataStream)> cb)
{
	assert(socket != nullptr); // Can't receive on a socket we don't own (external_socket)
	m_bComplete = false;
	DataStream d;
	d->Resize(size);

	nwm::cast_socket(*socket)->async_receive(boost::asio::buffer(d->GetData(), size), [this, d, cb](boost::system::error_code err, std::size_t) {
		std::lock_guard<std::mutex> guard(m_eventMutex);
		m_events.push([this, cb, err, d]() {
			m_bComplete = true;
			cb(err, d);
		});
	});
}

void UDPMessageDispatcher::Message::Dispatch()
{
	auto *s = (socket != nullptr) ? nwm::cast_socket(*socket) : nwm::cast_socket(*external_socket);
	s->async_send_to(boost::asio::buffer(data->GetData(), data->GetSize()), *cast_endpoint(endpoint), [this](boost::system::error_code err, std::size_t) {
		if(callback == nullptr)
			return;
		std::lock_guard<std::mutex> guard(m_eventMutex);
		m_events.push([this, err]() {
			m_bComplete = true;
			callback(err, this);
		});
	});
}

///////////////////////

UDPMessageDispatcher::DispatchInfo::DispatchInfo(DataStream &_data, const std::string &_ip, unsigned short _port, std::function<void(nwm::ErrorCode, Message *)> _callback) : data(_data), ip(_ip), port(_port), callback(_callback), valid_endpoint(false), socket(nullptr) {}

UDPMessageDispatcher::DispatchInfo::DispatchInfo(DataStream &_data, const nwm::UDPEndpoint &_ep, std::function<void(nwm::ErrorCode, UDPMessageDispatcher::Message *)> _callback) : data(_data), endpoint(_ep), callback(_callback), valid_endpoint(true), socket(nullptr) {}

UDPMessageDispatcher::DispatchInfo::DispatchInfo(DataStream &_data, const nwm::UDPEndpoint &_ep, nwm::UDPSocket &_socket, std::function<void(nwm::ErrorCode, UDPMessageDispatcher::Message *)> _callback)
    : data(_data), endpoint(_ep), callback(_callback), valid_endpoint(true), socket(&_socket)
{
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
