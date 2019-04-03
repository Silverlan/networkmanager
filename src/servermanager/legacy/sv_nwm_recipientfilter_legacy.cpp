/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "servermanager/sv_nwm_recipientfilter.h"
#include "servermanager/legacy/sv_nwm_serverclient.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
RecipientFilter::RecipientFilter(RPFilterType type)
	: m_type(type)
{}
void RecipientFilter::SetFilterType(RPFilterType type) {m_type = type;}
RPFilterType RecipientFilter::GetFilterType() const {return m_type;}
size_t RecipientFilter::GetRecipientCount() const {return m_sessions.size();}

void RecipientFilter::Add(const SessionHandle &hSession)
{
	for(auto it=m_sessions.begin();it!=m_sessions.end();it++)
	{
		SessionHandle &hOther = *it;
		if(hOther == hSession)
			return;
	}
	m_sessions.push_back(hSession);
}
void RecipientFilter::Remove(const SessionHandle &hSession)
{
	for(auto it=m_sessions.begin();it!=m_sessions.end();it++)
	{
		SessionHandle &hOther = *it;
		if(hOther == hSession)
		{
			m_sessions.erase(it);
			return;
		}
	}
}
void RecipientFilter::Add(NWMServerClient *client) {if(client == nullptr) return; Add(client->GetHandle());}
void RecipientFilter::Remove(NWMServerClient *client) {Remove(client->GetHandle());}
bool RecipientFilter::HasRecipient(const SessionHandle &hSession) {return HasRecipient(hSession.get());}
bool RecipientFilter::HasRecipient(NWMServerClient *client)
{
	for(auto it=m_sessions.begin();it!=m_sessions.end();it++)
	{
		SessionHandle &hSession = *it;
		if(hSession.IsValid() && hSession.get() == client)
			return true;
	}
	return false;
}
std::vector<SessionHandle> &RecipientFilter::get() {return m_sessions;}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
