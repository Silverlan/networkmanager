// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.server_manager;

import :legacy.session_handle;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif

SessionHandle::SessionHandle() : NWMServerClientHandle() { Initialize(); }
SessionHandle::SessionHandle(NWMServerClient *cl) : NWMServerClientHandle(cl) { Initialize(); }

SessionHandle::SessionHandle(const SessionHandle &hSession) : NWMServerClientHandle(const_cast<NWMServerClient*>(hSession.get())) { m_manager = hSession.GetManager(); }

bool SessionHandle::IsValid() const { return NWMServerClientHandle::IsValid(); }

bool SessionHandle::operator==(const SessionHandle &hOther) { return (get() == hOther.get()) ? true : false; }
bool SessionHandle::operator!=(const SessionHandle &hOther) { return (*this == hOther) ? false : true; }

void SessionHandle::Initialize()
{
	NWMServerClient *cl = get();
	if(cl == nullptr) {
		m_manager = nullptr;
		return;
	}
	m_manager = cl->GetManager();
}

NWMServer *SessionHandle::GetManager() const { return m_manager; }

/*SessionHandle *SessionHandle::Copy()
{
	return new SessionHandle(*this);
}*/

void SessionHandle::Drop(CLIENT_DROPPED reason) { m_manager->DropClient(get(), reason); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
