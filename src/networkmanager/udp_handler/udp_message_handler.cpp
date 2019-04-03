/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/udp_handler/udp_message_handler.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
UDPMessageHandler::UDPMessageHandler(unsigned short port)
	: UDPMessageDispatcher(),UDPMessageReceiver(port)
{}

UDPMessageHandler::~UDPMessageHandler()
{}

std::unique_ptr<UDPMessageHandler> UDPMessageHandler::Create(unsigned short port,unsigned int timeout)
{
	auto r = std::unique_ptr<UDPMessageHandler>(new UDPMessageHandler(port));
	r->SetTimeout(timeout);
	return r;
}

void UDPMessageHandler::Poll()
{
	UDPMessageDispatcher::Update(); // Skips the 'Poll' of UDPMessageBase, which will already be invoked by 'UDPMessageReceiver::Poll'
	UDPMessageReceiver::Poll();
}

void UDPMessageHandler::DispatchResponse(DataStream &data,nwm::UDPEndpoint &ep,std::function<void(nwm::ErrorCode,Message*)> callback)
{
	UDPMessageDispatcher::Dispatch(data,ep,m_socket,callback);
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
