/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UDP_MESSAGE_DISPATCHER_H__
#define __UDP_MESSAGE_DISPATCHER_H__

#include "networkmanager/nwm_boost.h"
#include "udp_message_base.h"
#include <sharedutils/datastream.h>
#include <queue>
#include <mutex>
#include <atomic>

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
		Message(boost::asio::io_service &ioService,std::function<void(boost::system::error_code,Message*)> callback=nullptr);
		Message(udp::socket &socket,std::function<void(boost::system::error_code,Message*)> callback=nullptr);
		~Message();
		std::unique_ptr<udp::socket> socket;
		udp::socket *external_socket;
		DataStream data;
		udp::endpoint endpoint;
		std::function<void(boost::system::error_code,Message*)> callback;
		bool IsActive() const;
		void Receive(unsigned int size,std::function<void(boost::system::error_code,DataStream)> callback);
	};
private:
	std::mutex m_eventMutex;
	std::queue<std::function<void()>> m_events;
protected:
	struct DispatchInfo
	{
		DispatchInfo(DataStream &data,const std::string &ip,unsigned short port,std::function<void(boost::system::error_code,Message*)> callback=nullptr);
		DispatchInfo(DataStream &data,const udp::endpoint &ep,std::function<void(boost::system::error_code,Message*)> callback=nullptr);
		DispatchInfo(DataStream &data,const udp::endpoint &ep,udp::socket &socket,std::function<void(boost::system::error_code,Message*)> callback=nullptr);
		DataStream data;
		std::string ip;
		unsigned short port;
		udp::endpoint endpoint;
		bool valid_endpoint;
		udp::socket *socket;
		std::function<void(boost::system::error_code,Message*)> callback;
	};
	UDPMessageDispatcher();
	std::atomic<unsigned int> m_active;
	std::recursive_mutex m_messageMutex;
	std::vector<std::unique_ptr<Message>> m_messages;
	std::mutex m_dispatchQueueMutex;
	std::queue<DispatchInfo> m_dispatchQueue;
	udp::resolver m_resolver;
	std::unique_ptr<udp::resolver::query> m_resolverQuery;
	boost::asio::deadline_timer m_deadline;
	unsigned int m_timeout;
	void ResolveNext(bool lockMutex=true);
	void CheckDeadline();
	void Cancel();
	void ResetTimeout();
	void Update();
	void Dispatch(const DispatchInfo &info);
	void ScheduleMessage(DispatchInfo &info,const udp::endpoint &ep);
	void Dispatch(DataStream &data,const udp::endpoint &ep,udp::socket &socket,std::function<void(boost::system::error_code,Message*)> callback=nullptr);
public:
	virtual ~UDPMessageDispatcher();
	static std::unique_ptr<UDPMessageDispatcher> Create(unsigned int timeout=0);
	void Dispatch(DataStream &data,const std::string &ip,unsigned short port,std::function<void(boost::system::error_code,Message*)> callback=nullptr);
	void Dispatch(DataStream &data,const udp::endpoint &ep,std::function<void(boost::system::error_code,Message*)> callback=nullptr);
	virtual void Poll() override;
	bool IsActive() const;
	unsigned int GetTimeout() const;
	void SetTimeout(unsigned int t);
};

#endif