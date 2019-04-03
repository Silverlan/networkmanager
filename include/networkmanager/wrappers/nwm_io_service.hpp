#ifndef __NWM_IO_SERVICE_HPP__
#define __NWM_IO_SERVICE_HPP__

#include "networkmanager/wrappers/nwm_boost_wrapper.hpp"

namespace boost{namespace asio{class io_context;};};
namespace nwm
{
	class IOService
		: public TBoostWrapper<boost::asio::io_context>
	{
	public:
		using TBoostWrapper<boost::asio::io_context>::TBoostWrapper;
	};
};

#endif
