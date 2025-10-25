// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>
#include <string>

export module pragma.server_manager:session.tcp;

export import :legacy.session;
export import pragma.network_manager;

export {
	class SVNWMTCPConnection;
	class NWMTCPSession : public NWMSession, public NWMTCPIO {
	  public:
		friend SVNWMTCPConnection;
	  protected:
		SVNWMTCPConnection *m_connection;
		virtual void OnPacketReceived() override;
		virtual void InitializeSharedPtr() override;
		virtual void OnTerminated() override;
	  public:
		NWMTCPSession(nwm::IOService &ioService, SVNWMTCPConnection *con);
		virtual ~NWMTCPSession() override;
		void Start();
		virtual bool IsConnectionActive() override;
		virtual bool IsUDP() const override;
		virtual const NWMEndpoint &GetRemoteEndPoint() const override;
		virtual void Close() override;
		virtual void SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket) override;
		virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
		virtual void Run() override;
		virtual std::string GetIP() const override;
		virtual unsigned short GetPort() const override;
		virtual nwm::IPAddress GetAddress() const override;
		virtual void SendPacket(const NetPacket &packet) override;
		virtual void SetTimeoutDuration(double duration) override;
		virtual void Release() override;
	};
}
