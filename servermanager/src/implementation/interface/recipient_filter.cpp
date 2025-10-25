// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <algorithm>
#include <vector>

module pragma.server_manager;

import :legacy.recipient_filter;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
nwm::RecipientFilter::RecipientFilter(Type type) : m_type(type) {}
void nwm::RecipientFilter::SetFilterType(Type type) { m_type = type; }
nwm::RecipientFilter::Type nwm::RecipientFilter::GetFilterType() const { return m_type; }
size_t nwm::RecipientFilter::GetRecipientCount() const { return m_sessions.size(); }

void nwm::RecipientFilter::Add(const ServerClientHandle &hSession)
{
	auto it = std::find(m_sessions.begin(), m_sessions.end(), hSession);
	if(it != m_sessions.end())
		return;
	m_sessions.push_back(hSession);
}
void nwm::RecipientFilter::Remove(const ServerClientHandle &hSession)
{
	auto it = std::find(m_sessions.begin(), m_sessions.end(), hSession);
	if(it == m_sessions.end())
		return;
	m_sessions.erase(it);
}
void nwm::RecipientFilter::Add(ServerClient *client)
{
	if(client == nullptr)
		return;
	Add(client->GetHandle());
}
void nwm::RecipientFilter::Remove(ServerClient *client) { Remove(client->GetHandle()); }
bool nwm::RecipientFilter::HasRecipient(const ServerClientHandle &hSession) const { return HasRecipient(hSession.get()); }
bool nwm::RecipientFilter::HasRecipient(const ServerClient *client) const
{
	auto it = std::find_if(m_sessions.begin(), m_sessions.end(), [client](const ServerClientHandle &hCl) { return (hCl.IsValid() && hCl.get() == client) ? true : false; });
	return (it != m_sessions.end()) ? true : false;
}
const std::vector<nwm::ServerClientHandle> &nwm::RecipientFilter::get() const { return const_cast<RecipientFilter *>(this)->get(); }
std::vector<nwm::ServerClientHandle> &nwm::RecipientFilter::get() { return m_sessions; }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
