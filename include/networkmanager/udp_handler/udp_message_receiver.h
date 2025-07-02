// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UDP_MESSAGE_RECEIVER_H__
#define __UDP_MESSAGE_RECEIVER_H__

#include "udp_message_base.h"
#include "networkmanager/wrappers/nwm_udp_endpoint.hpp"
#include "networkmanager/wrappers/nwm_udp_socket.hpp"
#include "networkmanager/wrappers/nwm_error_code.hpp"
#include <sharedutils/datastream.h>
#include <memory>
#include <functional>

class UDPMessageReceiver : virtual public UDPMessageBase {
  private:
	DataStream m_data;
	nwm::UDPEndpoint m_epOrigin;
  protected:
	UDPMessageReceiver(nwm::UDPEndpoint &ep);
	UDPMessageReceiver(unsigned short port);
	nwm::UDPSocket m_socket;
  public:
	virtual ~UDPMessageReceiver();
	void Receive(unsigned int size, const std::function<void(nwm::UDPEndpoint &, nwm::ErrorCode, DataStream)> &callback);
	static std::unique_ptr<UDPMessageReceiver> Create(unsigned short port);
};

#endif
