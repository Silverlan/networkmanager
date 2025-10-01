// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

export module pragma.network_manager:mutable_buffer;

export import :boost_wrapper;

export namespace nwm {
	class MutableBuffer : public TBoostWrapperCopyable<boost::asio::mutable_buffer> {
	public:
		MutableBuffer() :TBoostWrapperCopyable<boost::asio::mutable_buffer>{} {}
		MutableBuffer(const boost::asio::mutable_buffer &o) : TBoostWrapperCopyable<boost::asio::mutable_buffer>{o} {}
		MutableBuffer(const MutableBuffer &o) : TBoostWrapperCopyable<boost::asio::mutable_buffer>{o} {}
	};
};
