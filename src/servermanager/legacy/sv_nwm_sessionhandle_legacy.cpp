/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include "servermanager/legacy/sv_nwm_sessionhandle.h"
#include "servermanager/legacy/sv_nwm_manager.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
DEFINE_BASE_HANDLE(,NWMServerClient,NWMServerClient);

SessionHandle::SessionHandle()
	: NWMServerClientHandle()
{
	Initialize();
}
SessionHandle::SessionHandle(PtrNWMServerClient *t)
	: NWMServerClientHandle(t)
{
	Initialize();
}
SessionHandle::SessionHandle(NWMServerClient *cl)
	: NWMServerClientHandle(cl)
{
	Initialize();
}

SessionHandle::SessionHandle(const SessionHandle &hSession)
	: NWMServerClientHandle(hSession.get())
{
	m_manager = hSession.GetManager();
}

bool SessionHandle::IsValid() const {return NWMServerClientHandle::IsValid();}

bool SessionHandle::operator==(const SessionHandle &hOther) {return (get() == hOther.get()) ? true : false;}
bool SessionHandle::operator!=(const SessionHandle &hOther) {return (*this == hOther) ? false : true;}

void SessionHandle::Initialize()
{
	NWMServerClient *cl = get();
	if(cl == nullptr)
	{
		m_manager = nullptr;
		return;
	}
	m_manager = cl->GetManager();
}

NWMServer *SessionHandle::GetManager() const {return m_manager;}

void SessionHandle::Invalidate() {NWMServerClientHandle::Invalidate();}

/*SessionHandle *SessionHandle::Copy()
{
	return new SessionHandle(*this);
}*/

void SessionHandle::Drop(CLIENT_DROPPED reason)
{
	m_manager->DropClient(get(),reason);
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
