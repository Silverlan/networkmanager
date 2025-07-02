// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_DEADLINE_TIMER_HPP__
#define __NWM_DEADLINE_TIMER_HPP__

#include "networkmanager/wrappers/nwm_unique_void.hpp"

namespace nwm {
	class IOService;
	// Note: Can't use TBoostWrapper as base since boost tcp socket cannot be pre-declared
	class DeadlineTimer {
	  public:
		DeadlineTimer(nwm::IOService &ioService);

		const void *operator*() const;
		void *operator*();
		const void *GetBoostObject() const;
		void *GetBoostObject();
	  private:
		impl::unique_void_ptr m_deadlineTimer;
	};
};

#endif
