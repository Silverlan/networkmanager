// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/wrappers/nwm_deadline_timer.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"
#include "networkmanager/nwm_boost.h"

nwm::DeadlineTimer::DeadlineTimer(nwm::IOService &ioService) : m_deadlineTimer {impl::unique_void<boost::asio::deadline_timer>(new boost::asio::deadline_timer {*ioService})} {}

const void *nwm::DeadlineTimer::operator*() const { return const_cast<DeadlineTimer *>(this)->operator*(); }
void *nwm::DeadlineTimer::operator*() { return GetBoostObject(); }
const void *nwm::DeadlineTimer::GetBoostObject() const { return const_cast<DeadlineTimer *>(this)->GetBoostObject(); }
void *nwm::DeadlineTimer::GetBoostObject() { return m_deadlineTimer.get(); }
