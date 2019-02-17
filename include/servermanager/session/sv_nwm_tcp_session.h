/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_TCP_SESSION_H__
#define __SV_NWM_TCP_SESSION_H__

#include "networkmanager/io/nwm_tcp_io.h"
#include "servermanager/legacy/sv_nwm_session.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class SVNWMTCPConnection;
class NWMTCPSession
	: public NWMSession,public NWMTCPIO
{
public:
	friend SVNWMTCPConnection;
protected:
	SVNWMTCPConnection *m_connection;
	virtual void OnPacketReceived();
	virtual void InitializeSharedPtr() override;
	virtual void OnTerminated() override;
public:
	NWMTCPSession(boost::asio::io_service& ioService,SVNWMTCPConnection *con);
	virtual ~NWMTCPSession() override;
	void Start();
	virtual bool IsConnectionActive() override;
	virtual bool IsUDP() const override;
	virtual const NWMEndpoint &GetRemoteEndPoint() const override;
	virtual void Close() override;
	virtual void SetPacketHandle(const std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> &cbPacket) override;
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
	virtual void Run() override;
	virtual std::string GetIP() const override;
	virtual unsigned short GetPort() const override;
	virtual boost::asio::ip::address GetAddress() const override;
	virtual void SendPacket(const NetPacket &packet) override;
	virtual void SetTimeoutDuration(double duration) override;
	virtual void Release() override;
};

#endif