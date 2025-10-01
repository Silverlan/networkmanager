// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

export module pragma.network_manager:tcp.core;

export import :boost_wrapper;

export namespace nwm {
	class TCP : public TBoostWrapperCopyable<boost::asio::ip::tcp> {
	public:
		TCP() :TBoostWrapperCopyable<boost::asio::ip::tcp>{} {}
		TCP(const boost::asio::ip::tcp &o) : TBoostWrapperCopyable<boost::asio::ip::tcp>{o} {}
		TCP(const TCP &o) : TBoostWrapperCopyable<boost::asio::ip::tcp>{o} {}
	};
};
