// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWMCONNECTION_H__
#define __NWMCONNECTION_H__

#include <exception>
#include <string>
#include <queue>
#include <sharedutils/netpacket.hpp>
#include <functional>
#include "nwm_endpoint.h"
#include "wrappers/nwm_io_service.hpp"
#include "wrappers/nwm_ip_address.hpp"

class NWMConnection {
  protected:
	NWMConnection();
	virtual ~NWMConnection();
	bool m_bClosing;
	bool m_bShutDown;

	virtual void CloseSocket() = 0;
	virtual void Terminate();
  public:
	std::unique_ptr<nwm::IOService> ioService;
	virtual void Run();
	virtual void Close() = 0;
	virtual bool IsUDP() const;
	virtual bool IsTCP() const;
	virtual void SendPacket(const NetPacket &packet, const NWMEndpoint &ep, bool bOwn = false) = 0;
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose) = 0;
	virtual bool IsClosing() const = 0;
	virtual std::string GetLocalIP() const = 0;
	virtual unsigned short GetLocalPort() const = 0;
	virtual nwm::IPAddress GetLocalAddress() const = 0;
	void Poll();
	bool IsActive() const;
	virtual void SetTimeoutDuration(double duration) = 0;
};

#endif
