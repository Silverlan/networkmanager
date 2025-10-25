// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>
#include <queue>

#include <mutex>

module pragma.network_manager;

import :event_base;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMEventBase::NWMEventBase() : m_callStackMutex(new std::mutex), m_callStack(new std::queue<std::function<void()>>()) {}

NWMEventBase::~NWMEventBase() { ProcessEvents(); }

void NWMEventBase::ProcessEvents()
{
	auto mt = m_callStackMutex;
	mt->lock();
	auto callStack = m_callStack;
	while(!callStack->empty()) {
		auto &f = callStack->front();
		f();
		callStack->pop();
	}
	mt->unlock();
}

void NWMEventBase::ScheduleEvent(const std::function<void()> &f)
{
	m_callStackMutex->lock();
	m_callStack->push(f);
	m_callStackMutex->unlock();
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
