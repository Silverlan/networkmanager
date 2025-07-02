// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __SV_NWM_ACCEPTOR_HPP__
#define __SV_NWM_ACCEPTOR_HPP__

#include "networkmanager/wrappers/nwm_unique_void.hpp"

namespace nwm {
	class IOService;
	class TCPEndpoint;
	// Note: Can't use TBoostWrapper as base since boost acceptor cannot be pre-declared
	class TCPAcceptor {
	  public:
		TCPAcceptor(nwm::IOService &ioService, const TCPEndpoint &ep);

		const void *operator*() const;
		void *operator*();
		const void *GetBoostObject() const;
		void *GetBoostObject();
	  private:
		impl::unique_void_ptr m_acceptor;
	};
};

#endif
