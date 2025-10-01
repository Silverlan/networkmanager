// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <exception>
#include <string>
#include <queue>
#include <functional>
#include <memory>

export module pragma.network_manager:connection;

export import :endpoint;
export import :io_service;
export import pragma.util;

export class NWMConnection {
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
