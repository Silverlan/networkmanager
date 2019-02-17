/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/udp_handler/udp_message_dispatcher.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
UDPMessageDispatcher::Message::Message(boost::asio::io_service &ioService,std::function<void(boost::system::error_code,Message*)> _callback)
	: socket(std::make_unique<udp::socket>(ioService,udp::v4())),callback(_callback),m_bComplete(false),external_socket(nullptr)
{}

UDPMessageDispatcher::Message::Message(udp::socket &_socket,std::function<void(boost::system::error_code,Message*)> _callback)
	: external_socket(&_socket),callback(_callback),m_bComplete(false)
{}

UDPMessageDispatcher::Message::~Message()
{
	if(socket != nullptr)
		socket->close();
}

bool UDPMessageDispatcher::Message::IsActive() const {return !m_bComplete;}

void UDPMessageDispatcher::Message::Poll()
{
	m_eventMutex.lock();
		while(!m_events.empty())
		{
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
		socket->close();
}

void UDPMessageDispatcher::Message::Receive(unsigned int size,std::function<void(boost::system::error_code,DataStream)> cb)
{
	assert(socket != nullptr); // Can't receive on a socket we don't own (external_socket)
	m_bComplete = false;
	DataStream d;
	d->Resize(size);

	socket->async_receive(
		boost::asio::buffer(d->GetData(),size),
		[this,d,cb](boost::system::error_code err,std::size_t) {
			std::lock_guard<std::mutex> guard(m_eventMutex);
			m_events.push([this,cb,err,d]() {
				m_bComplete = true;
				cb(err,d);
			});
		}
	);
}

void UDPMessageDispatcher::Message::Dispatch()
{
	auto *s = (socket != nullptr) ? socket.get() : external_socket;
	s->async_send_to(
		boost::asio::buffer(data->GetData(),data->GetSize()),
		endpoint,
		[this](boost::system::error_code err,std::size_t) {
			if(callback == nullptr)
				return;
			std::lock_guard<std::mutex> guard(m_eventMutex);
			m_events.push([this,err]() {
				m_bComplete = true;
				callback(err,this);
			});
		}
	);
}

///////////////////////

UDPMessageDispatcher::DispatchInfo::DispatchInfo(DataStream &_data,const std::string &_ip,unsigned short _port,std::function<void(boost::system::error_code,Message*)> _callback)
	: data(_data),ip(_ip),port(_port),callback(_callback),valid_endpoint(false),socket(nullptr)
{}

UDPMessageDispatcher::DispatchInfo::DispatchInfo(DataStream &_data,const udp::endpoint &_ep,std::function<void(boost::system::error_code,UDPMessageDispatcher::Message*)> _callback)
	: data(_data),endpoint(_ep),callback(_callback),valid_endpoint(true),socket(nullptr)
{}

UDPMessageDispatcher::DispatchInfo::DispatchInfo(DataStream &_data,const udp::endpoint &_ep,udp::socket &_socket,std::function<void(boost::system::error_code,UDPMessageDispatcher::Message*)> _callback)
	: data(_data),endpoint(_ep),callback(_callback),valid_endpoint(true),socket(&_socket)
{}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
