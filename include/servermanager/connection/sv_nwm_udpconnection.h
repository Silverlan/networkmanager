/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_UDPCONNECTION_H__
#define __SV_NWM_UDPCONNECTION_H__

#include "networkmanager/nwm_udpconnection.h"
#include "sv_nwm_connection.h"
#include "servermanager/session/sv_nwm_udp_session.h"
#include "networkmanager/io/nwm_udp_io.h"

#define NWM_MESSAGE_IN_REGISTER_UDP 2
//#define NWM_MESSAGE_IN_REGISTER_TCP 3

class NWMServer;
class SVNWMUDPConnection
	: public NWMUDPConnection,public SVNWMConnection,public NWMUDPIO
{
public:
	friend NWMUDPSession;
protected:
	std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> m_packetSessionHandle;
	void HandleAccept(std::shared_ptr<NWMUDPSession> session,const boost::system::error_code &err);
	virtual void SendPacket(const NetPacket &packet,const NWMEndpoint &ep,bool bOwn=false) override;
	virtual void Accept() override;
	virtual void CloseSocket() override;
	virtual void ScheduleTermination() override;
	virtual bool HandleError(const boost::system::error_code &error) override;
	virtual void HandleReadHeader(const boost::system::error_code &err,std::size_t bytes) override;
	std::shared_ptr<NWMUDPSession> FindSession(const NWMUDPEndpoint &ep);
public:
	SVNWMUDPConnection(unsigned short localPort);
	virtual void Initialize() override;
	virtual ~SVNWMUDPConnection() override;
	virtual void Close() override;
	virtual bool IsClosing() const override;
	virtual void Terminate() override;
	virtual bool IsTerminated() const override;
	virtual void Run() override;
	void SetPacketHandle(const std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> &cbPacket);
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
	virtual void SetTimeoutDuration(double duration) override;
	virtual std::string GetLocalIP() const override;
	virtual unsigned short GetLocalPort() const override;
	virtual boost::asio::ip::address GetLocalAddress() const override;
};

#endif