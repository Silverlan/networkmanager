// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.network_manager:event_base;

export import std;

export class NWMEventBase {
  protected:
	NWMEventBase();
	std::shared_ptr<std::queue<std::function<void()>>> m_callStack;
	std::shared_ptr<std::mutex> m_callStackMutex;
	void ProcessEvents();
	void ScheduleEvent(const std::function<void()> &ev);
  public:
	virtual ~NWMEventBase();
};
