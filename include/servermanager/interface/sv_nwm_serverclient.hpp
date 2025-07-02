// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __SV_NWM_SERVERCLIENT_HPP__
#define __SV_NWM_SERVERCLIENT_HPP__

#include <memory>
#include <networkmanager/wrappers/nwm_ip_address.hpp>
#include "servermanager/interface/sv_nwm_manager.hpp"
#include "servermanager/interface/sv_nwm_clienthandle.hpp"

class NWMSession;
class NWMUDPSession;
class NWMTCPSession;
namespace nwm {
	enum class ClientDropped : int8_t;

	class ServerClient : public std::enable_shared_from_this<ServerClient> {
	  public:
		virtual ~ServerClient();
		ServerClientHandle GetHandle() const;

		bool IsTarget(const NWMEndpoint &ep) const;
		bool IsTarget(const nwm::IPAddress &address, uint16_t port) const;
		bool UsesAddress(const nwm::IPAddress &address) const;
		bool UsesPort(uint16_t port) const;
		void SetAddress(nwm::Protocol protocol, const nwm::IPAddress &address, uint16_t port);

		bool IsClosed() const;
		bool IsClosing() const;
		uint16_t GetLatency() const;
		void SetLatency(uint16_t latency);
		std::string GetIP() const;
		nwm::IPAddress GetAddress() const;
		uint16_t GetPort() const;
		NWMEndpoint GetRemoteEndpoint() const;
		std::string GetRemoteIP() const;
		nwm::IPAddress GetRemoteAddress() const;
		uint16_t GetRemotePort() const;
		ChronoDuration TimeSinceLastActivity() const;
		Server *GetManager() const;
		virtual bool IsUDPConnected() const;
		virtual bool IsTCPConnected() const;
		virtual bool IsFullyConnected() const;
		bool IsReady() const;
		void Close();
		void Drop(ClientDropped e);
	  protected:
		ServerClient(Server *manager);

		void SetIndex(size_t idx);
		size_t GetIndex() const;
		virtual void OnClosed() = 0;
		virtual void Release();
		void InitializeSessionData(NWMSession *session);
		void ResetLastUpdate();
		template<class T>
		void SessionCloseHandle(std::shared_ptr<T> &t);

		std::atomic<bool> m_bDropped = {false};
		std::atomic<uint16_t> m_latency = {0u};
		std::atomic<size_t> m_index = {0ull};
		Server *m_manager;

		struct Connection {
			std::atomic<bool> connected = {false};
			std::atomic<bool> initialized = {false};

			// NOT thread-safe
			NWMEndpoint remoteEndpoint;
			std::atomic<uint16_t> port = {0u};
			nwm::IPAddress address;
			std::string ip = nwm::invalid_address();
		};

		Connection m_conUDP = {};
		Connection m_conTCP = {};
		std::atomic<bool> m_bClosed = {false};
		std::atomic<bool> m_bReady = {false};

		mutable std::mutex m_lastUpdateMutex;
		ChronoTimePoint m_lastUpdate;

		// These are NOT thread-safe!
		// The 'protocol'-parameter is merely a preference and not a guarantee the specified protocol will be used!
		virtual void SendPacket(const NetPacket &p, nwm::Protocol protocol = nwm::Protocol::TCP);
		virtual void Run();
		void Terminate();
		void UpdateReadyState();
		std::shared_ptr<NWMUDPSession> m_udpSession = nullptr;
		std::shared_ptr<NWMTCPSession> m_tcpSession = nullptr;
		mutable std::mutex m_sessionDataMutex;
		ServerClientHandle m_handle = {};
	  private:
		std::atomic<bool> m_bClosing = {false};
		friend Server;

		// NOT thread-safe!
		void SetSession(NWMUDPSession *session);
		void SetSession(NWMTCPSession *session);
	};
};

#endif
