// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_MUTABLE_BUFFER_HPP__
#define __NWM_MUTABLE_BUFFER_HPP__

#include "networkmanager/wrappers/nwm_boost_wrapper.hpp"

namespace boost {
	namespace asio {
		class mutable_buffer;
	};
};
namespace nwm {
	class MutableBuffer : public TBoostWrapperCopyable<boost::asio::mutable_buffer> {
	  public:
		using TBoostWrapperCopyable<boost::asio::mutable_buffer>::TBoostWrapperCopyable;
	};
};

#endif
