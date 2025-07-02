// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_UDP_IO_H__
#define __NWM_UDP_IO_H__

#include "nwm_io.h"
#include "nwm_udp_io_base.h"
#include "networkmanager/wrappers/nwm_udp_socket.hpp"
#include "networkmanager/wrappers/nwm_mutable_buffer.hpp"
#include "networkmanager/wrappers/nwm_error_code.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"

class NWMUDPIO : public NWMIO, public NWMUDPIOBase {
  protected:
	NWMUDPIO(nwm::IOService &ioService, unsigned short localPort);
	std::unique_ptr<nwm::UDPSocket> m_socket;
	NWMEndpoint m_localEndpoint;
	virtual void Terminate() override;
	virtual void AsyncWrite(const NWMEndpoint &ep, const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f) override;
	virtual void AsyncRead(const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f, bool bPeek = false) override;
	virtual void CloseSocket();
  public:
	virtual ~NWMUDPIO() override;
	virtual std::string GetIP() const override;
	virtual nwm::IPAddress GetAddress() const override;
	virtual unsigned short GetPort() const override;
	virtual std::string GetLocalIP() const override;
	virtual nwm::IPAddress GetLocalAddress() const override;
	virtual unsigned short GetLocalPort() const override;
	virtual bool IsTerminated() const override;
	virtual nwm::Protocol GetProtocol() const override;
	NWMEndpoint &GetLocalEndPoint();
};

#endif
