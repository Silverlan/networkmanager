/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UDP_MESSAGE_HANDLER_H__
#define __UDP_MESSAGE_HANDLER_H__

#include "udp_message_dispatcher.h"
#include "udp_message_receiver.h"

class UDPMessageHandler
	: public UDPMessageDispatcher,
	public UDPMessageReceiver
{
protected:
	UDPMessageHandler(unsigned short port);
public:
	virtual ~UDPMessageHandler();
	virtual void Poll() override;
	// Send a response to a message we've received from a client
	void DispatchResponse(DataStream &data,nwm::UDPEndpoint &ep,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
	static std::unique_ptr<UDPMessageHandler> Create(unsigned short port,unsigned int timeout=0);
};

#endif