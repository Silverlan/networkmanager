// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <memory>
#include <string>
#include <sharedutils/def_handle.h>

export module pragma.server_manager:client_handle;

export import pragma.network_manager;

export namespace nwm {
	class ServerClient;
	DECLARE_BASE_HANDLE(, ServerClient, ServerClientBase);

	class Server;
	class ServerClientHandle : public nwm::ServerClientBaseHandle {
	  public:
		ServerClientHandle();
		ServerClientHandle(ServerClient *cl);
		ServerClientHandle(const ServerClientHandle &hSession);
		virtual ~ServerClientHandle() override;
		bool operator==(const ServerClientHandle &hOther) const;
		bool operator!=(const ServerClientHandle &hOther) const;
		Server *GetManager() const;
		void Drop(ClientDropped e);
		void Invalidate();
		bool IsValid() const;
	  protected:
		void Initialize();
		friend ServerClient;
	  private:
		mutable Server *m_manager = nullptr;
	};
};
