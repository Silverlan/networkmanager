// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

export module pragma.network_manager:udp;

export import :boost_wrapper;
export import :udp.core;
export import :udp.endpoint;
export import :udp.resolver;
export import :udp.resolver_query;
export import :udp.socket;
