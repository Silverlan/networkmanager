// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_IO_SERVICE_HPP__
#define __NWM_IO_SERVICE_HPP__

#include "networkmanager/wrappers/nwm_boost_wrapper.hpp"

namespace boost {
	namespace asio {
		class io_context;
	};
};
namespace nwm {
	class IOService : public TBoostWrapper<boost::asio::io_context> {
	  public:
		IOService();
	};
};

#endif
