// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.server_manager;

import :legacy.recipient_filter;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
RecipientFilter::RecipientFilter(RPFilterType type) : m_type(type) {}
void RecipientFilter::SetFilterType(RPFilterType type) { m_type = type; }
RPFilterType RecipientFilter::GetFilterType() const { return m_type; }
size_t RecipientFilter::GetRecipientCount() const { return m_sessions.size(); }

void RecipientFilter::Add(const SessionHandle &hSession)
{
	for(auto it = m_sessions.begin(); it != m_sessions.end(); it++) {
		SessionHandle &hOther = *it;
		if(hOther == hSession)
			return;
	}
	m_sessions.push_back(hSession);
}
void RecipientFilter::Remove(const SessionHandle &hSession)
{
	for(auto it = m_sessions.begin(); it != m_sessions.end(); it++) {
		SessionHandle &hOther = *it;
		if(hOther == hSession) {
			m_sessions.erase(it);
			return;
		}
	}
}
void RecipientFilter::Add(NWMServerClient *client)
{
	if(client == nullptr)
		return;
	Add(client->GetHandle());
}
void RecipientFilter::Remove(NWMServerClient *client) { Remove(client->GetHandle()); }
bool RecipientFilter::HasRecipient(const SessionHandle &hSession) { return HasRecipient(hSession.get()); }
bool RecipientFilter::HasRecipient(const NWMServerClient *client)
{
	for(auto it = m_sessions.begin(); it != m_sessions.end(); it++) {
		SessionHandle &hSession = *it;
		if(hSession.IsValid() && hSession.get() == client)
			return true;
	}
	return false;
}
std::vector<SessionHandle> &RecipientFilter::get() { return m_sessions; }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
