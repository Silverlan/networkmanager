// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_TCP_HPP__
#define __NWM_TCP_HPP__

#include "networkmanager/wrappers/nwm_boost_wrapper.hpp"

namespace boost {
	namespace asio {
		namespace ip {
			class tcp;
		};
	};
};
namespace nwm {
	class TCP : public TBoostWrapperCopyable<boost::asio::ip::tcp> {
	  public:
		using TBoostWrapperCopyable<boost::asio::ip::tcp>::TBoostWrapperCopyable;
	};
};

#endif
