#ifndef __NWM_IP_ADDRESS_HPP__
#define __NWM_IP_ADDRESS_HPP__

#include "networkmanager/wrappers/nwm_boost_wrapper.hpp"

namespace boost{namespace asio{namespace ip{class address;};};};
namespace nwm
{
	class IPAddress
		: public TBoostWrapperCopyable<boost::asio::ip::address>
	{
	public:
		using TBoostWrapperCopyable<boost::asio::ip::address>::TBoostWrapperCopyable;
		bool operator==(const IPAddress &other) const;
		bool operator!=(const IPAddress &other) const;
	};
};

#endif
