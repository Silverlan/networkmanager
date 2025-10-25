// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <functional>

export module pragma.network_manager:manager;

export import :enums;
export import :tcp_connection;
export import :udp_connection;
export import :ip_address;

export {
	namespace nwm {
		class Error {
		private:
			std::string m_message = "";
			int32_t m_value = -1;
		public:
			Error(int err, const std::string &msg);
			virtual const char *name() const;
			virtual std::string message() const;
			int32_t value() const;
		};

		namespace impl {
			class ManagerBase {
			public:
				enum class PollEventResult : uint8_t { Complete = 0, Pending };
				void Close();
				void Start();
				bool IsActive() const;
				bool IsClosing() const;
				void SetErrorHandle(const std::function<void(const Error &)> &cbError);

				virtual void PollEvents();
				bool HasUDPConnection() const;
				bool HasTCPConnection() const;
				virtual void SetTimeoutDuration(double duration);
				std::string GetLocalIP() const;
				uint16_t GetLocalPort() const;
				uint16_t GetLocalUDPPort() const;
				uint16_t GetLocalTCPPort() const;
				nwm::IPAddress GetLocalAddress() const;
				void SetNagleAlgorithmEnabled(bool b);
			protected:
				ManagerBase(const std::shared_ptr<NWMUDPConnection> &udp, const std::shared_ptr<NWMTCPConnection> &tcp);
				virtual ~ManagerBase();
				void InitializeConnectionData();

				void HandleError(const Error &err);
				void ScheduleAsyncEvent(const std::function<void(void)> &f);
				void SchedulePollEvent(const std::function<PollEventResult(void)> &f);

				// After calling this function the packet is no longer thread-safe!
				void SendPacket(Protocol protocol, const NetPacket &packet, NWMEndpoint &ep, bool bOwn = false);

				// NOT thread-safe!
				virtual void OnPacketSent(const NWMEndpoint &ep, const NetPacket &packet);
				virtual void OnClosed() = 0;
				virtual void Run() = 0;
				NWMUDPConnection *GetUDPConnection();
				NWMTCPConnection *GetTCPConnection();
				void CloseUDPConnection();
				void CloseTCPConnection();
				void CloseAsync();
			private:
				bool m_bHasTCP = false;
				bool m_bHasUDP = false;
				std::string m_localIp;
				uint16_t m_localUDPPort = 0;
				uint16_t m_localTCPPort = 0;
				nwm::IPAddress m_localAddress = {};
				std::function<void(const Error &)> m_cbError;
				std::unique_ptr<std::thread> m_thread = nullptr;
				std::atomic<bool> m_bActive = {false};

				std::atomic<bool> m_bHasThreadEvents = {false};
				std::atomic<bool> m_bHasPollEvents = {false};
				std::mutex m_threadEventMutex;
				std::mutex m_pollEventMutex;
				std::mutex m_eventMutex;

				// These are NOT thread-safe!
				std::shared_ptr<NWMUDPConnection> m_conUDP = nullptr;
				std::shared_ptr<NWMTCPConnection> m_conTCP = nullptr;
				std::queue<std::function<void(void)>> m_threadEventQueue;
				std::deque<std::function<PollEventResult(void)>> m_pollEventQueue;
			};
		};
	};
}
