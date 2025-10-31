// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.server_manager:legacy.server_client;

export import :legacy.session_handle;
export import :session.udp;
export import :session.tcp;
export import pragma.network_manager;

export {
	using ChronoTimePoint = util::Clock::time_point;
	using ChronoDuration = util::Clock::duration;

	class NWMServer;
	class NWMServerClient : public std::enable_shared_from_this<NWMServerClient> {
	  public:
		friend NWMServer;
	  protected:
		NWMServerClient(NWMServer *manager);
	  private:
		std::atomic<bool> m_bClosing;
	  protected:
		bool m_bDropped;
		std::atomic<uint16_t> m_latency;
		SessionHandle m_handle;
		NWMServer *m_manager;
		size_t m_index;
		bool m_bClosed;
		bool m_bTCPInitialized;
		bool m_bUDPInitialized;
		bool m_bReady;
		std::string m_ip;
		unsigned short m_port;
		nwm::IPAddress m_address;
		NWMEndpoint m_remoteEndPoint;
		std::shared_ptr<NWMUDPSession> m_udpSession;
		std::shared_ptr<NWMTCPSession> m_tcpSession;

		mutable std::mutex m_lastUpdateMutex;
		ChronoTimePoint m_lastUpdate;

		void UpdateReadyState();
		void SetEndPoint(const NWMEndpoint &ep);
		void SetIndex(size_t idx);
		size_t GetIndex() const;
		virtual void OnClosed() = 0;
		void Terminate();
		void InitializeSessionData(NWMSession *session);
		void ResetLastUpdate();
		template<class T>
		void SessionCloseHandle(std::shared_ptr<T> &t, bool bLock = true);
	  public:
		virtual ~NWMServerClient();
		virtual void Remove();
		SessionHandle GetHandle();
		//SessionHandle *CreateHandle();
		bool IsTarget(const NWMEndpoint &ep);
		bool IsTarget(const nwm::IPAddress &address, unsigned short port);
		bool IsClosed() const;
		bool IsClosing() const;
		unsigned short GetLatency() const;
		void SetLatency(unsigned short latency);
		std::string GetIP() const;
		nwm::IPAddress GetAddress() const;
		unsigned short GetPort() const;
		const NWMEndpoint &GetRemoteEndpoint() const;
		std::string GetRemoteIP() const;
		nwm::IPAddress GetRemoteAddress() const;
		unsigned short GetRemotePort() const;
		virtual void SendPacket(const NetPacket &p, bool bPreferUDP = false);
		void SendPacketTCP(const NetPacket &p);
		void SendPacketUDP(const NetPacket &p);
		void SetSession(NWMUDPSession *session);
		void SetSession(NWMTCPSession *session);
		ChronoDuration TimeSinceLastActivity() const;
		NWMServer *GetManager() const;
		virtual bool IsUDPConnected() const;
		virtual bool IsTCPConnected() const;
		virtual bool IsFullyConnected() const;
		bool IsReady() const;
		void Close();
		virtual void Run();
		void Drop(CLIENT_DROPPED reason);
	};
}
