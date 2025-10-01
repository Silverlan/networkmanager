// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <memory>
#include <string>
#include <sharedutils/def_handle.h>

export module pragma.server_manager:legacy.session_handle;

export import pragma.network_manager;

export {
	class NWMServerClient;
	DECLARE_BASE_HANDLE(, NWMServerClient, NWMServerClient);

	class NWMServer;
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
}
