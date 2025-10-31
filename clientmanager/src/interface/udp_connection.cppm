// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.client_manager:udp_connection;

export import :connection;

export class CLNWMUDPConnection : public NWMUDPConnection, public CLNWMConnection, public NWMUDPIO {
  protected:
	virtual void CloseSocket() override;
	virtual void SendPacket(const NetPacket &packet, const NWMEndpoint &ep, bool bOwn = false) override;
	virtual void OnTimedOut() override;
  public:
	CLNWMUDPConnection(unsigned short localPort);
	virtual ~CLNWMUDPConnection() override;
	NWMUDPEndpoint *GetRemoteEndPoint();
	std::string GetIP() const override;
	void Connect(std::string serverIp, unsigned int serverPort);
	virtual void Close() override;
	virtual bool IsClosing() const override;
	virtual void SendPacket(const NetPacket &packet, bool bOwn = false) override;
	virtual void Run() override;
	virtual void SetReady() override;
	virtual void SetTimeoutDuration(double duration) override;
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
	virtual std::string GetLocalIP() const override;
	virtual unsigned short GetLocalPort() const override;
	virtual nwm::IPAddress GetLocalAddress() const override;
};
