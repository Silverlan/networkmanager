// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_UDP_IO_BASE_H__
#define __NWM_UDP_IO_BASE_H__

#include "nwm_io.h"
#include "nwm_io_header.h"
#include "networkmanager/wrappers/nwm_mutable_buffer.hpp"
#include "networkmanager/wrappers/nwm_error_code.hpp"
#include "networkmanager/wrappers/nwm_udp_socket.hpp"
#include "networkmanager/wrappers/nwm_tcp_socket.hpp"

class NWMUDPIOBase {
  protected:
	virtual void AsyncWrite(nwm::UDPSocket *socket, const std::vector<nwm::MutableBuffer> &buffers, const NWMEndpoint &endPoint, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f);
	virtual void AsyncRead(nwm::UDPSocket *socket, const std::vector<nwm::MutableBuffer> &buffers, const NWMEndpoint &endPoint, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f, bool bPeek = false);
  public:
	NWMUDPIOBase();
	virtual ~NWMUDPIOBase();
	virtual bool IsClosing() const = 0;
	virtual std::string GetIP() const = 0;
	virtual unsigned short GetPort() const = 0;
	virtual void Terminate();
	virtual bool IsTerminated() const;
};

#endif
