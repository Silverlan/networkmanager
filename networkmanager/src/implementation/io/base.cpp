// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>
#include <chrono>

module pragma.network_manager;

import :io.base;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMIOBase::NWMIOBase() : std::enable_shared_from_this<NWMIOBase>(), NWMEventBase(), m_bClosing(false), m_bTerminated(false), m_bTerminating(false), m_remoteEndpoint(), m_localEndpoint(), m_tTimeout(0.0), m_bTimedOut(false), m_closeHandle(), m_lastError {}
{
	m_tLastMessage = util::Clock::now();
}

NWMIOBase::~NWMIOBase() {}

void NWMIOBase::SetTimeoutDuration(double duration, bool bIfConnectionActive)
{
	m_tTimeout = duration;
	m_tLastMessage = util::Clock::now(); // Make sure we're not causing a timeout right after this function has been called
}

const nwm::ErrorCode &NWMIOBase::GetLastError() const { return m_lastError; }
void NWMIOBase::OnError(const nwm::ErrorCode &error) { m_lastError = error; }

const NWMEndpoint &NWMIOBase::GetRemoteEndPoint() const { return m_remoteEndpoint; }
const NWMEndpoint &NWMIOBase::GetLocalEndPoint() const { return m_localEndpoint; }
bool NWMIOBase::IsTerminated() const { return m_bTerminated; }

bool NWMIOBase::ShouldTerminate() { return (m_bClosing == false || m_bTerminating == true || m_bTerminated == true) ? false : true; }

bool NWMIOBase::UpdateTermination()
{
	if(!ShouldTerminate())
		return false;
	ScheduleTermination();
	return true;
}

void NWMIOBase::OnTerminated() {}

void NWMIOBase::SetCloseHandle(const std::function<void(void)> &cbClose) { m_closeHandle = cbClose; }

void NWMIOBase::ScheduleTermination()
{
	m_bTerminating = true;
	m_bClosing = true;
}

void NWMIOBase::Terminate()
{
	if(m_bTerminated == true) // Don't use ::IsTerminated (virtual function)
		return;
	ProcessEvents();
	m_bClosing = false;
	m_bTerminated = true;
	m_bTerminating = false;
	OnTerminated();
	if(m_closeHandle != nullptr)
		m_closeHandle();
}

void NWMIOBase::Close(bool bForce)
{
	if(m_bTerminating == true || m_bClosing == true)
		return;
	m_bClosing = true;
	if(ShouldTerminate()) {
		if(bForce == true)
			ScheduleTermination();
		else
			UpdateTermination();
	}
}

void NWMIOBase::Close()
{
	if(m_bClosing == true)
		return;
	Close(false);
}

void NWMIOBase::Run()
{
	ProcessEvents();
	if(IsClosing()) {
		if(m_bTerminating == true) {
			Terminate();
			return;
		}
		UpdateTermination();
		return;
	}
	else if((m_bTimeoutIfConnectionActive == false || IsConnectionActive()) && m_tTimeout > 0.0 && IsReady() == true) {
		auto now = util::Clock::now();
		double t = std::chrono::duration<double>(now - m_tLastMessage).count();
		if(t > m_tTimeout) {
			m_bTimedOut = true;
			Close();
			OnTimedOut();
		}
	}
}

bool NWMIOBase::IsTimedOut() const { return m_bTimedOut; }
void NWMIOBase::OnTimedOut() {}

bool NWMIOBase::IsClosing() const { return m_bClosing; }
bool NWMIOBase::IsConnectionActive() { return (!m_bTerminated && !m_bTerminating) ? true : false; }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
