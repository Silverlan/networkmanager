/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/udp_handler/udp_message_receiver.h"
#include <boost/bind.hpp>

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
UDPMessageReceiver::UDPMessageReceiver(udp::endpoint &ep)
	: UDPMessageBase(),m_socket(m_ioService,ep)
{}

UDPMessageReceiver::UDPMessageReceiver(unsigned short port)
	: UDPMessageBase(),m_socket(m_ioService,udp::endpoint(udp::v4(),port))
{}

UDPMessageReceiver::~UDPMessageReceiver()
{
	m_socket.close();
}

std::unique_ptr<UDPMessageReceiver> UDPMessageReceiver::Create(unsigned short port)
{
	return std::unique_ptr<UDPMessageReceiver>(new UDPMessageReceiver(port));
}

void UDPMessageReceiver::Receive(unsigned int size,const std::function<void(udp::endpoint&,boost::system::error_code,DataStream)> &callback)
{
	m_data = DataStream();
	m_data->Resize(size);
	m_socket.async_receive_from(
		boost::asio::buffer(m_data->GetData(),size),
		m_epOrigin,
		[this,callback](boost::system::error_code err,std::size_t) {
			callback(m_epOrigin,err,m_data);
		}
	);
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
