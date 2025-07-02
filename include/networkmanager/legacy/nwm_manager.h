// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#include <queue>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <sharedutils/netpacket.hpp>
#include <sharedutils/util_clock.hpp>
#include "networkmanager/nwm_tcpconnection.h"
#include "networkmanager/nwm_udpconnection.h"

#define NETWORK_CON_TYPE_UDP 0
#define NETWORK_CON_TYPE_TCP 1
#define NETWORK_CON_TYPE_BOTH 2

#define NETWORK_ERROR_UNHANDLED_PACKET 10

class NWMError {
  private:
	std::string m_message;
	int m_value;
  public:
	NWMError(int err, const std::string &msg);
	virtual const char *name() const;
	virtual std::string message() const;
	int value() const;
};

#undef ERROR

enum class CLIENT_DROPPED { DISCONNECTED, TIMEOUT, KICKED, SHUTDOWN, ERROR };
std::string GetClientDroppedReasonString(CLIENT_DROPPED reason);

using ChronoTimePoint = util::Clock::time_point;
using ChronoDuration = util::Clock::duration;

class NWManagerBase {
  protected:
	struct SendPacketQueue {
		SendPacketQueue(const NetPacket &p, NWMEndpoint endPoint, bool bOwn, bool bTCP) : packet(p), ep(endPoint), own(bOwn), tcp(bTCP) {}
		NetPacket packet;
		NWMEndpoint ep;
		bool own;
		bool tcp;
	};
  private:
	void SendPackets();
  protected:
	NWManagerBase(const std::shared_ptr<NWMUDPConnection> &udp, const std::shared_ptr<NWMTCPConnection> &tcp);
	virtual ~NWManagerBase();
	void Poll();
	std::shared_ptr<NWMUDPConnection> m_conUDP;
	std::shared_ptr<NWMTCPConnection> m_conTCP;
	std::function<void(const NWMError &)> m_cbError;
	std::atomic<bool> m_bClosing;
	std::unique_ptr<std::thread> m_thread;

	std::mutex m_asyncPollQueueMutex;
	std::queue<std::function<void(void)>> m_asyncPollQueue;

	std::mutex m_pollQueueMutex;
	std::queue<std::function<void(void)>> m_pollQueue;

	std::queue<SendPacketQueue> m_sendPacketQueue;
	std::mutex m_sendPacketMutex;

	void QueueAsyncEvent(const std::function<void(void)> &f);
	void QueueEvent(const std::function<void(void)> &f);
	void PollAsyncEvents();
	void ReceivePacket(unsigned int id, const NetPacket &packet);
	void SendPacketTCP(const NetPacket &packet, NWMTCPEndpoint &ep, bool bOwn = false);
	void SendPacketUDP(const NetPacket &packet, NWMUDPEndpoint &ep, bool bOwn = false);
	void SendPacket(SendPacketQueue &item);
	void HandleError(const NWMError &err);
	virtual void OnClosed() = 0;
	virtual void SendPacket(const NetPacket &packet, NWMEndpoint &ep, bool bPreferUDP = false);
	virtual void CloseConnections();

	virtual void OnPacketSent(const NWMEndpoint &ep, const NetPacket &packet);
  public:
	NWMUDPConnection *GetUDPConnection() const;
	NWMTCPConnection *GetTCPConnection() const;
	virtual void Run();
	virtual void PollEvents();
	void Start();
	void StartAsync();
	bool IsAsync() const;
	bool IsActive() const;
	bool IsClosing() const;
	virtual void Close();
	bool HasUDPConnection() const;
	bool HasTCPConnection() const;
	void SetErrorHandle(const std::function<void(const NWMError &)> &cbError);
	virtual void SetTimeoutDuration(double duration);
	void Release();
	std::string GetLocalIP() const;
	unsigned short GetLocalPort() const;
	unsigned short GetLocalUDPPort() const;
	unsigned short GetLocalTCPPort() const;
	nwm::IPAddress GetLocalAddress() const;
	void SetNagleAlgorithmEnabled(bool b);
};

#endif
