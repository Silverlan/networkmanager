// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <memory>
#include <functional>

export module pragma.network_manager:udp_handler.message_receiver;

export import :error_code;
export import :udp.endpoint;
export import :udp_handler.core;
export import :udp.socket;
export import pragma.util;

export class UDPMessageReceiver : virtual public UDPMessageBase {
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
