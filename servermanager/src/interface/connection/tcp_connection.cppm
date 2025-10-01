// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>
#include <memory>
#include <string>

export module pragma.server_manager:connection.tcp;

export import :acceptor;
export import :connection.core;

export {
	class SVNWMTCPConnection : public NWMTCPConnection, public SVNWMConnection, public NWMEventBase, public NWMErrorHandle, public std::enable_shared_from_this<SVNWMTCPConnection> {
	  private:
		virtual void SendPacket(const NetPacket &packet, const NWMEndpoint &ep, bool bOwn = false) override;
	  protected:
		std::function<void(void)> m_closeHandle;
		std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> m_packetHandle;
		std::unique_ptr<nwm::TCPAcceptor> m_acceptor;
		bool m_bClosing;
		bool m_bTerminating;
		bool m_bNagleEnabled;
		virtual void Accept() override;
		void HandleAccept(NWMSessionHandle hSession, const nwm::ErrorCode &err);
		virtual void OnClientConnected(NWMSession *session) override;
		virtual void ScheduleTermination() override;
	  public:
		SVNWMTCPConnection(unsigned short minPort, unsigned short maxPort);
		SVNWMTCPConnection(unsigned short port);
		virtual void Initialize() override;
		virtual ~SVNWMTCPConnection() override;
		void SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket);
		virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
		virtual void Close() override;
		virtual void Run() override;
		virtual bool IsClosing() const override;
		virtual void Terminate() override;
		NWMEndpoint endPoint;
		virtual void SetTimeoutDuration(double duration) override;
		virtual std::string GetLocalIP() const override;
		virtual unsigned short GetLocalPort() const override;
		virtual nwm::IPAddress GetLocalAddress() const override;
		virtual void SetNagleAlgorithmEnabled(bool b) override;
	};
}
