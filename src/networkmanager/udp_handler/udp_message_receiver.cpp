// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/wrappers/nwm_impl_boost.hpp"
#include <boost/bind.hpp>
#include "networkmanager/udp_handler/udp_message_receiver.h"
#include "networkmanager/wrappers/nwm_udp_endpoint.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif

static udp::endpoint tmpUDPRecEndpoint {};
static udp::endpoint *create_boost_udp_endpoint(unsigned short port)
{
	tmpUDPRecEndpoint = udp::endpoint {udp::v4(), port};
	return &tmpUDPRecEndpoint;
}
UDPMessageReceiver::UDPMessageReceiver(nwm::UDPEndpoint &ep) : UDPMessageBase(), m_socket(m_ioService, ep) {}

UDPMessageReceiver::UDPMessageReceiver(unsigned short port) : UDPMessageBase(), m_socket(m_ioService, nwm::UDPEndpoint {create_boost_udp_endpoint(port)}) {}

UDPMessageReceiver::~UDPMessageReceiver() { nwm::cast_socket(m_socket)->close(); }

std::unique_ptr<UDPMessageReceiver> UDPMessageReceiver::Create(unsigned short port) { return std::unique_ptr<UDPMessageReceiver>(new UDPMessageReceiver(port)); }

void UDPMessageReceiver::Receive(unsigned int size, const std::function<void(nwm::UDPEndpoint &, nwm::ErrorCode, DataStream)> &callback)
{
	m_data = DataStream();
	m_data->Resize(size);
	nwm::cast_socket(m_socket)->async_receive_from(boost::asio::buffer(m_data->GetData(), size), *nwm::cast_endpoint(m_epOrigin), [this, callback](boost::system::error_code err, std::size_t) { callback(m_epOrigin, err, m_data); });
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
