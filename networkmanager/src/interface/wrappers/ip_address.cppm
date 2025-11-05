// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

export module pragma.network_manager:ip_address;

export import :boost_wrapper;

export namespace nwm {
	class IPAddress : public TBoostWrapperCopyable<boost::asio::ip::address> {
	  public:
		IPAddress();
		IPAddress(const boost::asio::ip::address &o) : TBoostWrapperCopyable<boost::asio::ip::address> {o} {}
		IPAddress(const IPAddress &o) : TBoostWrapperCopyable<boost::asio::ip::address> {o} {}

		bool operator==(const IPAddress &other) const;
		bool operator!=(const IPAddress &other) const;
		std::string ToString() const;
	};
};
