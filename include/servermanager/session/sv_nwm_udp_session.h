/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_UDP_SESSION_H__
#define __SV_NWM_UDP_SESSION_H__

#include "networkmanager/io/nwm_udp_io.h"
#include "servermanager/legacy/sv_nwm_session.h"

class SVNWMUDPConnection;
class NWMUDPSession : public NWMSession, public NWMIOBase, public NWMUDPIOBase {
  public:
	friend SVNWMUDPConnection;
  protected:
	SVNWMUDPConnection *m_connection;
	bool m_bReady;
	virtual void OnPacketReceived() override;
	virtual void InitializeSharedPtr() override;
  public:
	NWMUDPSession(const NWMEndpoint &ep, SVNWMUDPConnection *con);
	virtual ~NWMUDPSession() override;
	virtual bool IsUDP() const override;
	virtual const NWMEndpoint &GetRemoteEndPoint() const override;
	virtual const NWMEndpoint &GetLocalEndPoint() const override;
	virtual void Close() override;
	virtual void SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket) override;
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
	virtual std::string GetIP() const override;
	virtual unsigned short GetPort() const override;
	virtual nwm::IPAddress GetAddress() const override;
	virtual std::string GetLocalIP() const override;
	virtual unsigned short GetLocalPort() const override;
	virtual nwm::IPAddress GetLocalAddress() const override;
	virtual void Run() override;
	virtual void SendPacket(const NetPacket &packet) override;
	virtual bool IsTerminated() const override;

	virtual void SetReady() override;
	virtual bool IsReady() const override;
	virtual bool IsClosing() const override;
	virtual void SetTimeoutDuration(double duration) override;
	virtual void Release() override;
};

#endif
