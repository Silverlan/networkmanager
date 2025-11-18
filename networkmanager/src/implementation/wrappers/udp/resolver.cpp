// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

module pragma.network_manager;

import :udp.resolver;

nwm::UDPResolver::UDPResolver(nwm::IOService &ioService) : m_resolver {impl::unique_void<boost::asio::ip::udp::resolver>(new boost::asio::ip::udp::resolver {*ioService})} {}

const void *nwm::UDPResolver::operator*() const { return const_cast<UDPResolver *>(this)->operator*(); }
void *nwm::UDPResolver::operator*() { return GetBoostObject(); }
void *nwm::UDPResolver::GetBoostObject() { return m_resolver.get(); }
