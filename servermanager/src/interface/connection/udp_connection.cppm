// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.server_manager:connection.udp;

export import :connection.core;

export {
	constexpr uint32_t NWM_MESSAGE_IN_REGISTER_UDP = 2;
	//#define NWM_MESSAGE_IN_REGISTER_TCP 3
	class SVNWMUDPConnection : public NWMUDPConnection, public SVNWMConnection, public NWMUDPIO {
	  public:
		friend NWMUDPSession;
	  protected:
		std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> m_packetSessionHandle;
		void HandleAccept(std::shared_ptr<NWMUDPSession> session, const nwm::ErrorCode &err);
		virtual void SendPacket(const NetPacket &packet, const NWMEndpoint &ep, bool bOwn = false) override;
		virtual void Accept() override;
		virtual void CloseSocket() override;
		virtual void ScheduleTermination() override;
		virtual bool HandleError(const nwm::ErrorCode &error) override;
		virtual void HandleReadHeader(const nwm::ErrorCode &err, std::size_t bytes) override;
		std::shared_ptr<NWMUDPSession> FindSession(const NWMUDPEndpoint &ep);
	  public:
		SVNWMUDPConnection(unsigned short localPort);
		virtual void Initialize() override;
		virtual ~SVNWMUDPConnection() override;
		virtual void Close() override;
		virtual bool IsClosing() const override;
		virtual void Terminate() override;
		virtual bool IsTerminated() const override;
		virtual void Run() override;
		void SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket);
		virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
		virtual void SetTimeoutDuration(double duration) override;
		virtual std::string GetLocalIP() const override;
		virtual unsigned short GetLocalPort() const override;
		virtual nwm::IPAddress GetLocalAddress() const override;
	};
}
