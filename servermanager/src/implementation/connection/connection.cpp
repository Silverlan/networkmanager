// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>

module pragma.server_manager;

import :connection.core;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
SVNWMConnection::SVNWMConnection() : m_tSessionTimeout(0.0) {}

void SVNWMConnection::OnClientConnected(NWMSession *session)
{
	if(m_clientHandle != nullptr)
		m_clientHandle(session);
}

NWMSession *SVNWMConnection::GetSession(const nwm::UDPEndpoint &ep)
{
	for(auto it = m_sessions.begin(); it != m_sessions.end(); it++) {
		NWMSession *session = it->get();
		if(session->IsTarget(cast_endpoint(ep)->address(), cast_endpoint(ep)->port()))
			return session;
	}
	return nullptr;
}

NWMSessionHandle SVNWMConnection::AddSession(const std::shared_ptr<NWMSession> &session)
{
	session->SetTimeoutDuration(m_tSessionTimeout);
	m_sessions.push_back(session->GetHandle());
	return session->GetHandle();
}

void SVNWMConnection::RemoveSession(NWMSession *session)
{
	for(auto it = m_sessions.begin(); it != m_sessions.end(); it++) {
		if(it->get() == session) {
			m_sessions.erase(it);
			break;
		}
	}
	AttemptTermination();
}

void SVNWMConnection::SetClientHandle(const std::function<void(NWMSession *)> &cbClient) { m_clientHandle = cbClient; }
void SVNWMConnection::SetAcceptCallbacks(const AcceptCallbacks &callbacks) { m_acceptCallbacks = callbacks; }

void SVNWMConnection::Run()
{
	if(!m_sessions.empty()) {
		size_t i = m_sessions.size();
		while(i > 0) {
			i--;
			m_sessions[i]->Run();
		}
	}
}

void SVNWMConnection::SetTimeoutDuration(double duration)
{
	m_tSessionTimeout = duration;
	for(auto it = m_sessions.begin(); it != m_sessions.end(); it++)
		(*it)->SetTimeoutDuration(duration);
}

void SVNWMConnection::AttemptTermination()
{
	if(!IsClosing() || !m_sessions.empty())
		return;
	ScheduleTermination();
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
