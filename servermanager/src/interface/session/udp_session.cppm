// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.server_manager:session.udp;

export import :legacy.session;
export import pragma.network_manager;

export {
	class SVNWMUDPConnection;
	class NWMUDPSession : public NWMSession, public NWMIOBase, public NWMUDPIOBase {
	  public:
		friend SVNWMUDPConnection;
	  protected:
		SVNWMUDPConnection *m_connection;
		bool m_bReady;
		virtual void OnPacketReceived() override;
		virtual void InitializeSharedPtr() override;
	  public:
		NWMUDPSession(const NWMEndpoint &ep, SVNWMUDPConnection *con);
		virtual ~NWMUDPSession() override;
		virtual bool IsUDP() const override;
		virtual const NWMEndpoint &GetRemoteEndPoint() const override;
		virtual const NWMEndpoint &GetLocalEndPoint() const override;
		virtual void Close() override;
		virtual void SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket) override;
		virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
		virtual std::string GetIP() const override;
		virtual unsigned short GetPort() const override;
		virtual nwm::IPAddress GetAddress() const override;
		virtual std::string GetLocalIP() const override;
		virtual unsigned short GetLocalPort() const override;
		virtual nwm::IPAddress GetLocalAddress() const override;
		virtual void Run() override;
		virtual void SendPacket(const NetPacket &packet) override;
		virtual bool IsTerminated() const override;

		virtual void SetReady() override;
		virtual bool IsReady() const override;
		virtual bool IsClosing() const override;
		virtual void SetTimeoutDuration(double duration) override;
		virtual void Release() override;
	};
}
