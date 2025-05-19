/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
