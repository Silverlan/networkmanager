// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __SV_NWM_IMPL_SOCKET_HPP__
#define __SV_NWM_IMPL_SOCKET_HPP__

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "sv_nwm_acceptor.hpp"

namespace nwm {
	inline const boost::asio::ip::tcp::acceptor *cast_acceptor(const nwm::TCPAcceptor &acceptor) { return cast_acceptor(const_cast<nwm::TCPAcceptor &>(acceptor)); }
	inline boost::asio::ip::tcp::acceptor *cast_acceptor(nwm::TCPAcceptor &acceptor) { return static_cast<boost::asio::ip::tcp::acceptor *>(*acceptor); }
};

#endif
