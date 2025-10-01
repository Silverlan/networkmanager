// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_BOOST_H__
#define __NWM_BOOST_H__

#pragma warning(disable : 4005)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#pragma warning(default : 4005)

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

#endif
