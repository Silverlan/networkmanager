/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
