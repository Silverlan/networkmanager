// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_TCP_IO_H__
#define __NWM_TCP_IO_H__

#include "nwm_io.h"
#include "networkmanager/wrappers/nwm_error_code.hpp"
#include "networkmanager/wrappers/nwm_mutable_buffer.hpp"
#include "networkmanager/wrappers/nwm_ip_address.hpp"
#include "networkmanager/wrappers/nwm_tcp_socket.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"

class NWMTCPIO : public NWMIO {
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

#endif
