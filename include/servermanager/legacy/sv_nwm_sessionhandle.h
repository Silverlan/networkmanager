// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __SV_NWM_SESSIONHANDLE_H__
#define __SV_NWM_SESSIONHANDLE_H__

#include <memory>
#include <string>
#include <sharedutils/def_handle.h>

class NWMServerClient;
DECLARE_BASE_HANDLE(, NWMServerClient, NWMServerClient);

class NWMServer;
enum class CLIENT_DROPPED;
class SessionHandle : public NWMServerClientHandle {
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
