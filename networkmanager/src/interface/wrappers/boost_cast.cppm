// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

export module pragma.network_manager:boost_cast;

export import :deadline_timer;
export import :tcp.endpoint;
export import :tcp.socket;
export import :udp.resolver;
export import :udp.socket;

export {
	namespace nwm {
		inline const boost::asio::ip::tcp::socket *cast_socket(const TCPSocket &socket) { return static_cast<const boost::asio::ip::tcp::socket *>(*socket); }
		inline boost::asio::ip::tcp::socket *cast_socket(TCPSocket &socket) { return static_cast<boost::asio::ip::tcp::socket *>(*socket); }
		inline const boost::asio::ip::udp::socket *cast_socket(const UDPSocket &socket) { return static_cast<const boost::asio::ip::udp::socket *>(*socket); }
		inline boost::asio::ip::udp::socket *cast_socket(UDPSocket &socket) { return static_cast<boost::asio::ip::udp::socket *>(*socket); }
		inline const boost::asio::ip::tcp::endpoint *cast_endpoint(const TCPEndpoint &endpoint) { return static_cast<const boost::asio::ip::tcp::endpoint *>(*endpoint); }
		inline boost::asio::ip::tcp::endpoint *cast_endpoint(TCPEndpoint &endpoint) { return static_cast<boost::asio::ip::tcp::endpoint *>(*endpoint); }
		inline const boost::asio::ip::udp::endpoint *cast_endpoint(const UDPEndpoint &endpoint) { return static_cast<const boost::asio::ip::udp::endpoint *>(*endpoint); }
		inline boost::asio::ip::udp::endpoint *cast_endpoint(UDPEndpoint &endpoint) { return static_cast<boost::asio::ip::udp::endpoint *>(*endpoint); }
		inline const boost::asio::ip::udp::resolver *cast_resolver(const UDPResolver &resolver) { return static_cast<const boost::asio::ip::udp::resolver *>(*resolver); }
		inline boost::asio::ip::udp::resolver *cast_resolver(UDPResolver &resolver) { return static_cast<boost::asio::ip::udp::resolver *>(*resolver); }
		inline const boost::asio::steady_timer *cast_deadline_timer(const DeadlineTimer &deadline_timer) { return static_cast<const boost::asio::steady_timer *>(*deadline_timer); }
		inline boost::asio::steady_timer *cast_deadline_timer(DeadlineTimer &deadline_timer) { return static_cast<boost::asio::steady_timer *>(*deadline_timer); }

		std::string to_string(const boost::asio::ip::udp::endpoint &ep);
		std::string to_string(const boost::asio::ip::tcp::endpoint &ep);
	};
}
