/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
