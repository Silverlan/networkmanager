// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "boost/asio/ip/tcp.hpp"

module pragma.server_manager;

import :acceptor;

nwm::TCPAcceptor::TCPAcceptor(nwm::IOService &ioService, const TCPEndpoint &ep) : m_acceptor {impl::unique_void<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor {*ioService, *cast_endpoint(ep)})} {}

const void *nwm::TCPAcceptor::operator*() const { return const_cast<TCPAcceptor *>(this)->operator*(); }
void *nwm::TCPAcceptor::operator*() { return GetBoostObject(); }
const void *nwm::TCPAcceptor::GetBoostObject() const { return const_cast<TCPAcceptor *>(this)->GetBoostObject(); }
void *nwm::TCPAcceptor::GetBoostObject() { return m_acceptor.get(); }
