/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UDP_MESSAGE_DISPATCHER_H__
#define __UDP_MESSAGE_DISPATCHER_H__

#include "udp_message_base.h"
#include "networkmanager/wrappers/nwm_error_code.hpp"
#include "networkmanager/wrappers/nwm_udp_socket.hpp"
#include "networkmanager/wrappers/nwm_udp_endpoint.hpp"
#include "networkmanager/wrappers/nwm_udp_resolver.hpp"
#include "networkmanager/wrappers/nwm_udp_resolver_query.hpp"
#include "networkmanager/wrappers/nwm_deadline_timer.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"
#include <sharedutils/datastream.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>

class UDPMessageDispatcher
	: virtual public UDPMessageBase
{
public:
	struct Message
	{
	public:
		friend UDPMessageDispatcher;
	private:
		std::mutex m_eventMutex;
		std::queue<std::function<void()>> m_events;
		bool m_bComplete;
	protected:
		void Dispatch();
		void Poll();
		void Cancel();
	public:
		Message(nwm::IOService &ioService,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
		Message(nwm::UDPSocket &socket,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
		~Message();
		std::unique_ptr<nwm::UDPSocket> socket;
		nwm::UDPSocket *external_socket;
		DataStream data;
		nwm::UDPEndpoint endpoint;
		std::function<void(nwm::ErrorCode,Message*)> callback;
		bool IsActive() const;
		void Receive(unsigned int size,std::function<void(nwm::ErrorCode,DataStream)> callback);
	};
private:
	std::mutex m_eventMutex;
	std::queue<std::function<void()>> m_events;
protected:
	struct DispatchInfo
	{
		DispatchInfo(DataStream &data,const std::string &ip,unsigned short port,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
		DispatchInfo(DataStream &data,const nwm::UDPEndpoint &ep,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
		DispatchInfo(DataStream &data,const nwm::UDPEndpoint &ep,nwm::UDPSocket &socket,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
		DataStream data;
		std::string ip;
		unsigned short port;
		nwm::UDPEndpoint endpoint;
		bool valid_endpoint;
		nwm::UDPSocket *socket;
		std::function<void(nwm::ErrorCode,Message*)> callback;
	};
	UDPMessageDispatcher();
	std::atomic<unsigned int> m_active;
	std::recursive_mutex m_messageMutex;
	std::vector<std::unique_ptr<Message>> m_messages;
	std::mutex m_dispatchQueueMutex;
	std::queue<DispatchInfo> m_dispatchQueue;
	nwm::UDPResolver m_resolver;
	std::unique_ptr<nwm::UDPResolverQuery> m_resolverQuery;
	nwm::DeadlineTimer m_deadline;
	unsigned int m_timeout;
	void ResolveNext(bool lockMutex=true);
	void CheckDeadline();
	void Cancel();
	void ResetTimeout();
	void Update();
	void Dispatch(const DispatchInfo &info);
	void ScheduleMessage(DispatchInfo &info,const nwm::UDPEndpoint &ep);
	void Dispatch(DataStream &data,const nwm::UDPEndpoint &ep,nwm::UDPSocket &socket,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
public:
	virtual ~UDPMessageDispatcher();
	static std::unique_ptr<UDPMessageDispatcher> Create(unsigned int timeout=0);
	void Dispatch(DataStream &data,const std::string &ip,unsigned short port,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
	void Dispatch(DataStream &data,const nwm::UDPEndpoint &ep,std::function<void(nwm::ErrorCode,Message*)> callback=nullptr);
	virtual void Poll() override;
	bool IsActive() const;
	unsigned int GetTimeout() const;
	void SetTimeout(unsigned int t);
};

#endif