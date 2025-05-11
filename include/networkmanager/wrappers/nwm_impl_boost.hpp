#ifndef __NWM_IMPL_SOCKET_HPP__
#define __NWM_IMPL_SOCKET_HPP__

#include "nwm_tcp_socket.hpp"
#include "nwm_udp_socket.hpp"
#include "nwm_tcp_endpoint.hpp"
#include "nwm_udp_endpoint.hpp"
#include "nwm_udp_resolver.hpp"
#include "nwm_udp_resolver_query.hpp"
#include "nwm_deadline_timer.hpp"
#include "../nwm_boost.h"

namespace nwm {
	inline const boost::asio::ip::tcp::socket *cast_socket(const nwm::TCPSocket &socket) { return static_cast<const boost::asio::ip::tcp::socket *>(*socket); }
	inline boost::asio::ip::tcp::socket *cast_socket(nwm::TCPSocket &socket) { return static_cast<boost::asio::ip::tcp::socket *>(*socket); }
	inline const boost::asio::ip::udp::socket *cast_socket(const nwm::UDPSocket &socket) { return static_cast<const boost::asio::ip::udp::socket *>(*socket); }
	inline boost::asio::ip::udp::socket *cast_socket(nwm::UDPSocket &socket) { return static_cast<boost::asio::ip::udp::socket *>(*socket); }
	inline const boost::asio::ip::tcp::endpoint *cast_endpoint(const nwm::TCPEndpoint &endpoint) { return static_cast<const boost::asio::ip::tcp::endpoint *>(*endpoint); }
	inline boost::asio::ip::tcp::endpoint *cast_endpoint(nwm::TCPEndpoint &endpoint) { return static_cast<boost::asio::ip::tcp::endpoint *>(*endpoint); }
	inline const boost::asio::ip::udp::endpoint *cast_endpoint(const nwm::UDPEndpoint &endpoint) { return static_cast<const boost::asio::ip::udp::endpoint *>(*endpoint); }
	inline boost::asio::ip::udp::endpoint *cast_endpoint(nwm::UDPEndpoint &endpoint) { return static_cast<boost::asio::ip::udp::endpoint *>(*endpoint); }
	inline const boost::asio::ip::udp::resolver *cast_resolver(const nwm::UDPResolver &resolver) { return static_cast<const boost::asio::ip::udp::resolver *>(*resolver); }
	inline boost::asio::ip::udp::resolver *cast_resolver(nwm::UDPResolver &resolver) { return static_cast<boost::asio::ip::udp::resolver *>(*resolver); }
	inline const boost::asio::deadline_timer *cast_deadline_timer(const nwm::DeadlineTimer &deadline_timer) { return static_cast<const boost::asio::deadline_timer *>(*deadline_timer); }
	inline boost::asio::deadline_timer *cast_deadline_timer(nwm::DeadlineTimer &deadline_timer) { return static_cast<boost::asio::deadline_timer *>(*deadline_timer); }

	std::string to_string(const boost::asio::ip::udp::endpoint &ep);
	std::string to_string(const boost::asio::ip::tcp::endpoint &ep);
};

#endif
