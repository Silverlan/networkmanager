// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

module pragma.network_manager;

import :tcp.socket;

nwm::TCPSocket::TCPSocket(nwm::IOService &ioService) : m_socket {impl::unique_void<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket {*ioService})} {}

const void *nwm::TCPSocket::operator*() const { return const_cast<TCPSocket *>(this)->operator*(); }
void *nwm::TCPSocket::operator*() { return GetBoostObject(); }
void *nwm::TCPSocket::GetBoostObject() { return m_socket.get(); }
