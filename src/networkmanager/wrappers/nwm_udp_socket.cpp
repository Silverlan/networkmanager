// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/wrappers/nwm_udp_socket.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"
#include "networkmanager/nwm_boost.h"
#include "networkmanager/wrappers/nwm_impl_boost.hpp"

nwm::UDPSocket::UDPSocket(nwm::IOService &ioService) : m_socket {impl::unique_void<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket {*ioService})} {}
nwm::UDPSocket::UDPSocket(nwm::IOService &ioService, const nwm::UDP &udp) : m_socket {impl::unique_void<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket {*ioService, *udp})} {}
nwm::UDPSocket::UDPSocket(nwm::IOService &ioService, const nwm::UDPEndpoint &udp) : m_socket {impl::unique_void<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket {*ioService, *cast_endpoint(udp)})} {}

const void *nwm::UDPSocket::operator*() const { return const_cast<UDPSocket *>(this)->operator*(); }
void *nwm::UDPSocket::operator*() { return GetBoostObject(); }
void *nwm::UDPSocket::GetBoostObject() { return m_socket.get(); }
