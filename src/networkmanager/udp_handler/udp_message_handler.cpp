// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/udp_handler/udp_message_handler.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
UDPMessageHandler::UDPMessageHandler(unsigned short port) : UDPMessageDispatcher(), UDPMessageReceiver(port) {}

UDPMessageHandler::~UDPMessageHandler() {}

std::unique_ptr<UDPMessageHandler> UDPMessageHandler::Create(unsigned short port, unsigned int timeout)
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

void UDPMessageHandler::DispatchResponse(DataStream &data, nwm::UDPEndpoint &ep, std::function<void(nwm::ErrorCode, Message *)> callback) { UDPMessageDispatcher::Dispatch(data, ep, m_socket, callback); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
