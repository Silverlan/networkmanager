/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UDP_MESSAGE_RECEIVER_H__
#define __UDP_MESSAGE_RECEIVER_H__

#include "networkmanager/nwm_boost.h"
#include "udp_message_base.h"
#include <sharedutils/datastream.h>
#include <memory>

class UDPMessageReceiver
	: virtual public UDPMessageBase
{
private:
	DataStream m_data;
	udp::endpoint m_epOrigin;
protected:
	UDPMessageReceiver(udp::endpoint &ep);
	UDPMessageReceiver(unsigned short port);
	udp::socket m_socket;
public:
	virtual ~UDPMessageReceiver();
	void Receive(unsigned int size,const std::function<void(udp::endpoint&,boost::system::error_code,DataStream)> &callback);
	static std::unique_ptr<UDPMessageReceiver> Create(unsigned short port);
};

#endif