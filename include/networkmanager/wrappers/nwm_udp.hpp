#ifndef __NWM_UDP_HPP__
#define __NWM_UDP_HPP__

#include "networkmanager/wrappers/nwm_boost_wrapper.hpp"

namespace boost{namespace asio{namespace ip{class udp;};};};
namespace nwm
{
	class UDP
		: public TBoostWrapperCopyable<boost::asio::ip::udp>
	{
	public:
		using TBoostWrapperCopyable<boost::asio::ip::udp>::TBoostWrapperCopyable;
	};
};

#endif
