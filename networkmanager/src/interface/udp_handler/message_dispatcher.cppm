// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <queue>
#include <mutex>
#include <atomic>
#include <memory>
#include <functional>

export module pragma.network_manager:udp_handler.message_dispatcher;

export import :deadline_timer;
export import :error_code;
export import :io_service;
export import :udp_handler.core;
export import :udp.resolver;
export import :udp.resolver_query;
export import :udp.socket;
export import pragma.util;

export {
	class UDPMessageDispatcher : virtual public UDPMessageBase {
	public:
		struct Message {
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
			Message(nwm::IOService &ioService, std::function<void(nwm::ErrorCode, Message *)> callback = nullptr);
			Message(nwm::UDPSocket &socket, std::function<void(nwm::ErrorCode, Message *)> callback = nullptr);
			~Message();
			std::unique_ptr<nwm::UDPSocket> socket;
			nwm::UDPSocket *external_socket;
			util::DataStream data;
			nwm::UDPEndpoint endpoint;
			std::function<void(nwm::ErrorCode, Message *)> callback;
			bool IsActive() const;
			void Receive(unsigned int size, std::function<void(nwm::ErrorCode, util::DataStream)> callback);
		};
	private:
		std::mutex m_eventMutex;
		std::queue<std::function<void()>> m_events;
	protected:
		struct DispatchInfo {
			DispatchInfo(util::DataStream &data, const std::string &ip, unsigned short port, std::function<void(nwm::ErrorCode, Message *)> callback = nullptr);
			DispatchInfo(util::DataStream &data, const nwm::UDPEndpoint &ep, std::function<void(nwm::ErrorCode, Message *)> callback = nullptr);
			DispatchInfo(util::DataStream &data, const nwm::UDPEndpoint &ep, nwm::UDPSocket &socket, std::function<void(nwm::ErrorCode, Message *)> callback = nullptr);
			util::DataStream data;
			std::string ip;
			unsigned short port;
			nwm::UDPEndpoint endpoint;
			bool valid_endpoint;
			nwm::UDPSocket *socket;
			std::function<void(nwm::ErrorCode, Message *)> callback;
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
		void ResolveNext(bool lockMutex = true);
		void CheckDeadline();
		void Cancel();
		void ResetTimeout();
		void Update();
		void Dispatch(const DispatchInfo &info);
		void ScheduleMessage(DispatchInfo &info, const nwm::UDPEndpoint &ep);
		void Dispatch(util::DataStream &data, const nwm::UDPEndpoint &ep, nwm::UDPSocket &socket, std::function<void(nwm::ErrorCode, Message *)> callback = nullptr);
	public:
		virtual ~UDPMessageDispatcher();
		static std::unique_ptr<UDPMessageDispatcher> Create(unsigned int timeout = 0);
		void Dispatch(util::DataStream &data, const std::string &ip, unsigned short port, std::function<void(nwm::ErrorCode, Message *)> callback = nullptr);
		void Dispatch(util::DataStream &data, const nwm::UDPEndpoint &ep, std::function<void(nwm::ErrorCode, Message *)> callback = nullptr);
		virtual void Poll() override;
		bool IsActive() const;
		unsigned int GetTimeout() const;
		void SetTimeout(unsigned int t);
	};
}
