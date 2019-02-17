/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CLIENTMANAGER_H__
#define __CLIENTMANAGER_H__

#pragma warning(disable: 4307)
#include "networkmanager/nwm_boost.h"
#pragma warning(default: 4307)
#include "networkmanager/legacy/nwm_manager.h"
#include "clientmanager/cl_nwm_tcpconnection.h"
#include "clientmanager/cl_nwm_udpconnection.h"
#include "networkmanager/nwm_packet.h"
#include <queue>
#include <functional>
#include <atomic>

#define NWM_CLIENT_PING_INTERVAL 2

class NWMServerInfo
{
private:
	unsigned int m_numClients;
	unsigned int m_maxClients;
public:
	NWMServerInfo();
	unsigned int GetClientCount() const;
	unsigned int GetMaxClients() const;
	virtual void Read(NetPacket &packet);
	virtual void Write(NetPacket &packet);
};

class NWMClient
	: public NWManagerBase
{
protected:
	NWMClient(const std::shared_ptr<CLNWMUDPConnection> &udp,const std::shared_ptr<CLNWMTCPConnection> &tcp);
	virtual ~NWMClient() override;
	virtual void Initialize(const std::string &serverIp,unsigned short serverPort);
	std::atomic<bool> m_bPingEnabled;

	std::mutex m_lastPingMutex;
	ChronoTimePoint m_tLastPing;

	boost::system::error_code m_lastError;
	std::array<uint16_t,5> m_latencies;
	std::atomic<uint16_t> m_latency;

	// Called on thead!
	virtual bool HandlePacket(NWMIO *io,unsigned int id,NetPacket &packet);

	virtual void OnConnected()=0;
	virtual void OnDisconnected(CLIENT_DROPPED reason)=0;
	template<class T>
		static std::unique_ptr<T> Create(const std::string &serverIp,unsigned short serverPort,unsigned short localPort=0,unsigned char conType=NETWORK_CON_TYPE_UDP);
	void SendPacketTCP(const NetPacket &packet,bool bOwn);
	void SendPacketUDP(const NetPacket &packet,bool bOwn);
	virtual void Close() override;
	virtual void CloseConnections() override;

	// Called before HandlePacket is called
	virtual void OnPacketReceived(NWMIO *io,unsigned int id,NetPacket &packet);

	void Ping();
	void Pong();
public:
	//static NWMClient *Create(const std::string &serverIp,unsigned int serverPort,unsigned int minClientPort,unsigned int maxClientPort,unsigned char conType=NETWORK_CON_TYPE_UDP);
	virtual void Run() override;
	void SendPacketTCP(const NetPacket &packet);
	void SendPacketUDP(const NetPacket &packet);
	std::string GetIP();
	void Disconnect();
	unsigned short GetLatency() const;
	const boost::system::error_code &GetLastError() const;
	void SetPingEnabled(bool b);
};

template<class T>
	std::unique_ptr<T> NWMClient::Create(const std::string &serverIp,unsigned short serverPort,unsigned short localPort,unsigned char conType)
{
	std::shared_ptr<CLNWMUDPConnection> udp;
	if(conType == NETWORK_CON_TYPE_UDP || conType == NETWORK_CON_TYPE_BOTH)
	{
		try
		{
			udp = std::shared_ptr<CLNWMUDPConnection>(new CLNWMUDPConnection(localPort));
		}
		catch(NWMException &e)
		{
			throw e;
		}
	}
	std::shared_ptr<CLNWMTCPConnection> tcp;
	if(conType == NETWORK_CON_TYPE_TCP || conType == NETWORK_CON_TYPE_BOTH)
	{
		try
		{
			tcp = std::shared_ptr<CLNWMTCPConnection>(new CLNWMTCPConnection);
		}
		catch(NWMException &e)
		{
			throw e;
		}
	}
	auto r = std::unique_ptr<T>(new T(udp,tcp));
	try
	{
		r->Initialize(serverIp,serverPort);
	}
	catch(std::exception &e)
	{
		udp = nullptr;
		tcp = nullptr;
		r->Release();
		throw NWMException(e.what());
	}
	return r;
}

#endif