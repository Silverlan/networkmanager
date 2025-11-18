// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

module pragma.network_manager;

import :io_service;

using namespace nwm;

IOService::IOService() : TBoostWrapper<boost::asio::io_context> {} { m_boostBaseObject = impl::unique_void<boost::asio::io_context>(new boost::asio::io_context {}); }
