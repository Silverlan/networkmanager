// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.client_manager:tcp_connection;

export import :connection;

export class CLNWMTCPConnection : public NWMTCPConnection, public CLNWMConnection, public NWMTCPIO {
  protected:
	std::function<void(void)> m_connectCallback;
	void OnConnect(const nwm::ErrorCode &err);
	virtual void CloseSocket() override;
	virtual void SendPacket(const NetPacket &packet, const NWMEndpoint &ep, bool bOwn = false) override;
	virtual void OnTimedOut() override;
	void HandleOnConnect(const nwm::ErrorCode &err);
  public:
	CLNWMTCPConnection();
	virtual ~CLNWMTCPConnection() override;
	NWMTCPEndpoint *GetRemoteEndPoint();
	virtual void SendPacket(const NetPacket &packet, bool bOwn = false) override;
	void Connect(std::string serverIp, unsigned short serverPort);
	virtual void Close() override;
	virtual bool IsClosing() const override;
	virtual void Run() override;
	virtual void SetReady() override;
	virtual void SetTimeoutDuration(double duration) override;
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
	virtual std::string GetLocalIP() const override;
	virtual unsigned short GetLocalPort() const override;
	virtual nwm::IPAddress GetLocalAddress() const override;
	virtual void SetNagleAlgorithmEnabled(bool b) override;
	void SetConnectCallback(const std::function<void(void)> &f);
};
