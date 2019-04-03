#ifndef __NWM_UDP_SOCKET_HPP__
#define __NWM_UDP_SOCKET_HPP__

#include "networkmanager/wrappers/nwm_unique_void.hpp"
#include "networkmanager/wrappers/nwm_udp.hpp"
#include "networkmanager/wrappers/nwm_udp_endpoint.hpp"

namespace nwm
{
	class IOService;
	// Note: Can't use TBoostWrapper as base since boost tcp socket cannot be pre-declared
	class UDPSocket
	{
	public:
		UDPSocket(nwm::IOService &ioService);
		UDPSocket(nwm::IOService &ioService,const nwm::UDP &udp);
		UDPSocket(nwm::IOService &ioService,const nwm::UDPEndpoint &udp);
		
		const void *operator*() const;
		void *operator*();
		void *GetBoostObject();
	private:
		impl::unique_void_ptr m_socket;
	};
};

#endif
