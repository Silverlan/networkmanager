/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CL_NWM_MANAGER_HPP__
#define __CL_NWM_MANAGER_HPP__

#include <networkmanager/interface/nwm_manager.hpp>
#include <mutex>
#include <array>
#include "clientmanager/cl_nwm_tcpconnection.h"
#include "clientmanager/cl_nwm_udpconnection.h"

namespace nwm
{
	class Client
		: public impl::ManagerBase
	{
	public:
		virtual ~Client() override;
		void Disconnect();
		uint16_t GetLatency() const;
		const std::string &GetIP() const;
		const boost::system::error_code &GetLastError() const;
		const NWMEndpoint &GetRemoteEndpoint() const;
		void SetPingEnabled(bool b);
		void SendPacket(nwm::Protocol protocol,const NetPacket &packet);

		template<class T>
			static std::unique_ptr<T> Create(const std::string &serverIp,uint16_t serverPort,uint16_t localPort=0,nwm::ConnectionType type=nwm::ConnectionType::UDP);
	protected:
		Client(const std::shared_ptr<CLNWMUDPConnection> &udp,const std::shared_ptr<CLNWMTCPConnection> &tcp);
		virtual void Initialize(const std::string &serverIp,uint16_t serverPort);
		void SendPacket(nwm::Protocol protocol,const NetPacket &packet,bool bOwn);
		virtual bool HandlePacket(const NWMEndpoint &ep,uint32_t id,NetPacket &packet);
		// Called before HandlePacket is called
		virtual void OnPacketReceived(const NWMEndpoint &ep,unsigned int id,NetPacket &packet);

		virtual void OnConnected()=0;
		virtual void OnDisconnected(nwm::ClientDropped reason)=0;

		// NOT thread-safe!
		virtual void Run() override;
		CLNWMUDPConnection *GetUDPConnection();
		CLNWMTCPConnection *GetTCPConnection();
	private:
		static const uint32_t PING_INTERVAL;
		void Disconnect(ClientDropped reason);

		std::atomic<bool> m_bPingEnabled = {true};
		std::mutex m_lastPingMutex;
		ChronoTimePoint m_tLastPing;

		NWMEndpoint m_remoteEndpoint;
		boost::system::error_code m_lastError;
		std::array<uint16_t,5> m_latencies;
		std::atomic<uint16_t> m_latency = {0u};
		std::string m_ip;

		// NOT thread-safe!
		void Ping();
		void Pong();
	};
};

template<class T>
	std::unique_ptr<T> nwm::Client::Create(const std::string &serverIp,uint16_t serverPort,uint16_t localPort,nwm::ConnectionType type)
{
	std::shared_ptr<CLNWMUDPConnection> udp;
	if(type == ConnectionType::UDP || type == ConnectionType::TCPUDP)
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
	if(type == ConnectionType::TCP || type == ConnectionType::TCPUDP)
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
		throw NWMException(e.what());
	}
	return r;
}

#endif
