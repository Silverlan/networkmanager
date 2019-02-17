/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_SESSIONHANDLE_H__
#define __SV_NWM_SESSIONHANDLE_H__

#include <memory>
#include <string>
#include "networkmanager/nwm_boost.h"
#include <sharedutils/def_handle.h>

class NWMServerClient;
DECLARE_BASE_HANDLE(,NWMServerClient,NWMServerClient);

class NWMServer;
enum class CLIENT_DROPPED;
class SessionHandle
	: public NWMServerClientHandle
{
public:
	friend NWMServerClient;
	friend NWMServer;
private:
	NWMServer *m_manager;
protected:
	void Initialize();
public:
	SessionHandle();
	SessionHandle(PtrNWMServerClient *t);
	SessionHandle(NWMServerClient *cl);
	SessionHandle(const SessionHandle &hSession);
	bool operator==(const SessionHandle &hOther);
	bool operator!=(const SessionHandle &hOther);
	NWMServer *GetManager() const;
	void Drop(CLIENT_DROPPED reason);
	//virtual SessionHandle *Copy() override;
	void Invalidate();
	bool IsValid() const;
};

#endif