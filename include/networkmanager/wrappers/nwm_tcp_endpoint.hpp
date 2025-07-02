// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_TCP_ENDPOINT_HPP__
#define __NWM_TCP_ENDPOINT_HPP__

#include "networkmanager/wrappers/nwm_unique_void.hpp"

namespace nwm {
	class IOService;
	// Note: Can't use TBoostWrapper as base since boost tcp endpoint cannot be pre-declared
	class TCPEndpoint {
	  public:
		TCPEndpoint(const TCPEndpoint &ep);
		TCPEndpoint(void *boostTCPEndpoint);
		TCPEndpoint();

		const void *operator*() const;
		void *operator*();
		const void *GetBoostObject() const;
		void *GetBoostObject();
	  private:
		impl::unique_void_ptr m_endPoint;
	};
};

#endif
