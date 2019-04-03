#ifndef __NWM_UDP_ENDPOINT_HPP__
#define __NWM_UDP_ENDPOINT_HPP__

#include "networkmanager/wrappers/nwm_unique_void.hpp"

namespace nwm
{
	class IOService;
	// Note: Can't use TBoostWrapper as base since boost udp endpoint cannot be pre-declared
	class UDPEndpoint
	{
	public:
		UDPEndpoint(const UDPEndpoint &ep);
		UDPEndpoint(void *boostUDPEndpoint);
		UDPEndpoint();
		UDPEndpoint &operator=(const UDPEndpoint &other);
		
		const void *operator*() const;
		void *operator*();
		const void *GetBoostObject() const;
		void *GetBoostObject();
	private:
		impl::unique_void_ptr m_endPoint;
	};
};

#endif
