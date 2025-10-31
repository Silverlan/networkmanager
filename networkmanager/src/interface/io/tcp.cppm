// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.network_manager:io.tcp;

export import :io.core;
export import :tcp.socket;

export class NWMTCPIO : public NWMIO {
  protected:
	NWMTCPIO(nwm::IOService &ioService);
	virtual void AsyncWrite(const NWMEndpoint &ep, const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f) override;
	virtual void AsyncRead(const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f, bool bPeek = false) override;
	virtual void Terminate() override;
	bool m_bNagleEnabled;
	bool m_bSocketInitialized;
  public:
	virtual ~NWMTCPIO() override;
	std::unique_ptr<nwm::TCPSocket> socket;
	virtual std::string GetIP() const override;
	virtual unsigned short GetPort() const override;
	virtual nwm::IPAddress GetAddress() const override;
	virtual std::string GetLocalIP() const override;
	virtual unsigned short GetLocalPort() const override;
	virtual nwm::IPAddress GetLocalAddress() const override;
	virtual bool IsTerminated() const override;
	virtual nwm::Protocol GetProtocol() const override;
	void InitializeSocket();
	virtual void SetNagleAlgorithmEnabled(bool b);
};
