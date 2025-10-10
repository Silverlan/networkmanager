// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>
#include <string>
#include "sharedutils/util_shared_handle.hpp"

export module pragma.server_manager:legacy.session;

export import pragma.network_manager;

export {
	class NWMSession;
	using NWMSessionHandle = util::TSharedHandle<NWMSession>;

	class NWMSession {
	  protected:
		NWMSession();
		NWMSessionHandle m_handle;
		virtual void OnPacketReceived() = 0;
		virtual void InitializeSharedPtr() = 0;
	  public:
		virtual ~NWMSession();
		void Initialize();
		virtual bool IsUDP() const = 0;
		bool IsTCP() const;
		virtual void Release();
		virtual const NWMEndpoint &GetRemoteEndPoint() const = 0;
		virtual void SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket) = 0;
		virtual void Close() = 0;
		virtual void Run() = 0;
		virtual void SendPacket(const NetPacket &packet) = 0;
		virtual std::string GetIP() const = 0;
		virtual unsigned short GetPort() const = 0;
		virtual nwm::IPAddress GetAddress() const = 0;
		bool IsTarget(const NWMEndpoint &ep);
		bool IsTarget(const nwm::IPAddress &address, unsigned short port);
		virtual void SetTimeoutDuration(double duration) = 0;
		virtual void SetCloseHandle(const std::function<void(void)> &cbClose) = 0;
		NWMSessionHandle GetHandle();
	};
}
