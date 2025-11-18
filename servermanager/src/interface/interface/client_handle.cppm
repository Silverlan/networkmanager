// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.server_manager:client_handle;

export import pragma.network_manager;

export namespace nwm {
	class ServerClient;
	class Server;
	using ServerClientBaseHandle = util::TSharedHandle<ServerClient>;
	class ServerClientHandle : public ServerClientBaseHandle {
	  public:
		ServerClientHandle();
		ServerClientHandle(ServerClient *cl);
		ServerClientHandle(const ServerClientHandle &hSession);
		~ServerClientHandle();
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
