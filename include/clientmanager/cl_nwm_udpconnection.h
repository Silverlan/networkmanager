/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CL_NWM_UDPCONNECTION_H__
#define __CL_NWM_UDPCONNECTION_H__

#include "networkmanager/nwm_udpconnection.h"
#include "cl_nwm_connection.h"
#include "networkmanager/io/nwm_udp_io.h"

class CLNWMUDPConnection : public NWMUDPConnection, public CLNWMConnection, public NWMUDPIO {
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

#endif
