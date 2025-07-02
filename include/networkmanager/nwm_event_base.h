// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_EVENT_BASE_H__
#define __NWM_EVENT_BASE_H__

#include <mutex>
#include <functional>
#include <queue>
#include <memory>

class NWMEventBase {
  protected:
	NWMEventBase();
	std::shared_ptr<std::queue<std::function<void()>>> m_callStack;
	std::shared_ptr<std::mutex> m_callStackMutex;
	void ProcessEvents();
	void ScheduleEvent(const std::function<void()> &ev);
  public:
	virtual ~NWMEventBase();
};

#endif
