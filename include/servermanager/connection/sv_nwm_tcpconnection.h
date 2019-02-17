/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_TCPCONNECTION_H__
#define __SV_NWM_TCPCONNECTION_H__

#include "networkmanager/nwm_tcpconnection.h"
#include "sv_nwm_connection.h"
#include "servermanager/session/sv_nwm_tcp_session.h"
#include "servermanager/legacy/sv_nwm_serverclient.h"

class SVNWMTCPConnection
	: public NWMTCPConnection,public SVNWMConnection,public NWMEventBase,
	public NWMErrorHandle,public std::enable_shared_from_this<SVNWMTCPConnection>
{
private:
	virtual void SendPacket(const NetPacket &packet,const NWMEndpoint &ep,bool bOwn=false) override;
protected:
	std::function<void(void)> m_closeHandle;
	std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> m_packetHandle;
	std::unique_ptr<tcp::acceptor> m_acceptor;
	bool m_bClosing;
	bool m_bTerminating;
	bool m_bNagleEnabled;
	virtual void Accept() override;
	void HandleAccept(NWMSessionHandle hSession,const boost::system::error_code &err);
	virtual void OnClientConnected(NWMSession *session) override;
	virtual void ScheduleTermination() override;
public:
	SVNWMTCPConnection(unsigned short minPort,unsigned short maxPort);
	SVNWMTCPConnection(unsigned short port);
	virtual void Initialize() override;
	virtual ~SVNWMTCPConnection() override;
	void SetPacketHandle(const std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> &cbPacket);
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose) override;
	virtual void Close() override;
	virtual void Run() override;
	virtual bool IsClosing() const override;
	virtual void Terminate() override;
	NWMEndpoint endPoint;
	virtual void SetTimeoutDuration(double duration) override;
	virtual std::string GetLocalIP() const override;
	virtual unsigned short GetLocalPort() const override;
	virtual boost::asio::ip::address GetLocalAddress() const override;
	void SetNagleAlgorithmEnabled(bool b);
};

#endif