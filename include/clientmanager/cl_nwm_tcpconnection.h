/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CL_NWM_TCPCONNECTION_H__
#define __CL_NWM_TCPCONNECTION_H__

#include "networkmanager/nwm_tcpconnection.h"
#include "cl_nwm_connection.h"
#include "networkmanager/io/nwm_tcp_io.h"
#include "networkmanager/wrappers/nwm_error_code.hpp"

class CLNWMTCPConnection
	: public NWMTCPConnection,public CLNWMConnection,
	public NWMTCPIO
{
protected:
	std::function<void(void)> m_connectCallback;
	void OnConnect(const nwm::ErrorCode &err);
	virtual void CloseSocket() override;
	virtual void SendPacket(const NetPacket &packet,const NWMEndpoint &ep,bool bOwn=false) override;
	virtual void OnTimedOut() override;
	void HandleOnConnect(const nwm::ErrorCode &err);
public:
	CLNWMTCPConnection();
	virtual ~CLNWMTCPConnection() override;
	NWMTCPEndpoint *GetRemoteEndPoint();
	virtual void SendPacket(const NetPacket &packet,bool bOwn=false) override;
	void Connect(std::string serverIp,unsigned short serverPort);
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

#endif