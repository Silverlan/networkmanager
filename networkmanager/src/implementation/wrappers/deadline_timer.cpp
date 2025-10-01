// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

module pragma.network_manager;

import :deadline_timer;

nwm::DeadlineTimer::DeadlineTimer(nwm::IOService &ioService) : m_deadlineTimer {impl::unique_void<boost::asio::steady_timer>(new boost::asio::steady_timer {*ioService})} {}

const void *nwm::DeadlineTimer::operator*() const { return const_cast<DeadlineTimer *>(this)->operator*(); }
void *nwm::DeadlineTimer::operator*() { return GetBoostObject(); }
const void *nwm::DeadlineTimer::GetBoostObject() const { return const_cast<DeadlineTimer *>(this)->GetBoostObject(); }
void *nwm::DeadlineTimer::GetBoostObject() { return m_deadlineTimer.get(); }
