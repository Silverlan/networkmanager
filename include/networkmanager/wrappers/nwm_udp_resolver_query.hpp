#ifndef __NWM_UDP_RESOLVER_QUERY_HPP__
#define __NWM_UDP_RESOLVER_QUERY_HPP__

#include "networkmanager/wrappers/nwm_unique_void.hpp"
#include <string>

namespace nwm
{
	class IOService;
	// Note: Can't use TBoostWrapper as base since boost tcp socket cannot be pre-declared
	class UDPResolverQuery
	{
	public:
		UDPResolverQuery(const std::string &ip,const std::string &port);
		
		const void *operator*() const;
		void *operator*();
		void *GetBoostObject();
	private:
		impl::unique_void_ptr m_resolverQuery;
	};
};

#endif
