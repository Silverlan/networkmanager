// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>
#include <string>

export module pragma.network_manager:error_code;

export import :boost_wrapper;

export namespace nwm {
	class ErrorCode : public TBoostWrapperCopyable<boost::system::error_code> {
	  public:
		ErrorCode() : TBoostWrapperCopyable<boost::system::error_code>{} {}
		ErrorCode(const boost::system::error_code &o) : TBoostWrapperCopyable<boost::system::error_code>{o} {}

		ErrorCode(const ErrorCode &other);
		operator bool() const;
		bool operator!() const;
		std::string Message() const;

		// This is required to shut gcc up about some
		// weird linking issues
		ErrorCode &operator=(const ErrorCode &other);
		ErrorCode &operator=(const boost::system::error_code &o);
	};
};
