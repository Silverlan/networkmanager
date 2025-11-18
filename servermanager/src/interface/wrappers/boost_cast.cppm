// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

export module pragma.server_manager:boost_cast;

export import :acceptor;

export namespace nwm {
	inline const boost::asio::ip::tcp::acceptor *cast_acceptor(const nwm::TCPAcceptor &acceptor) { return cast_acceptor(const_cast<nwm::TCPAcceptor &>(acceptor)); }
	inline boost::asio::ip::tcp::acceptor *cast_acceptor(nwm::TCPAcceptor &acceptor) { return static_cast<boost::asio::ip::tcp::acceptor *>(*acceptor); }
};
