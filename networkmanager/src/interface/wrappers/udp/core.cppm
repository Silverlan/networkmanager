// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

export module pragma.network_manager:udp.core;

export import :boost_wrapper;

export namespace nwm {
	class UDP : public TBoostWrapperCopyable<boost::asio::ip::udp> {
	  public:
		UDP() : TBoostWrapperCopyable<boost::asio::ip::udp> {} {}
		UDP(const boost::asio::ip::udp &o) : TBoostWrapperCopyable<boost::asio::ip::udp> {o} {}
		UDP(const UDP &o) : TBoostWrapperCopyable<boost::asio::ip::udp> {o} {}
	};
};
