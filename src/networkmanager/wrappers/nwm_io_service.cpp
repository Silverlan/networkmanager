// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/wrappers/nwm_io_service.hpp"
#include "networkmanager/nwm_boost.h"

using namespace nwm;

IOService::IOService() : TBoostWrapper<boost::asio::io_context> {} { m_boostBaseObject = impl::unique_void<boost::asio::io_context>(new boost::asio::io_context {}); }
