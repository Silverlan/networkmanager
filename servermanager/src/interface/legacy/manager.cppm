// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>
#include <functional>
#include <queue>
#include <mutex>

export module pragma.server_manager:legacy.manager;

export import :connection.tcp;
export import :connection.udp;
export import :legacy.recipient_filter;
export import :legacy.server_client;
export import pragma.network_manager;

export {
	uint32_t NWM_MESSAGE_IN_PING = 1;
	uint32_t NWM_MESSAGE_IN_CLIENT_DISCONNECT = 4;

	uint32_t NWM_MESSAGE_OUT_PONG = 51;
	uint32_t NWM_MESSAGE_OUT_REGISTER_CONFIRMATION = 52;
	uint32_t NWM_MESSAGE_OUT_DROPPED = 53;

	uint32_t NWM_MESSAGE_IN_RECEIVE_MESSAGE = 11;

	uint32_t NWM_MESSAGE_OUT_SEND_SERVER_LIST = 3;
	uint32_t NWM_MESSAGE_OUT_SEND_MESSAGE = 12;

	class NWMServer : public NWManagerBase {
	  public:
		friend NWMServerClient;
		friend SessionHandle;
	  private:
		virtual void SendPacket(const NetPacket &packet, NWMEndpoint &ep, bool bPreferUDP = false) override;
		void RemoveClient(std::vector<std::shared_ptr<NWMServerClient>> &clients, NWMServerClient *cl);
	  protected:
		int m_maxClients;
		NWMServerClient *FindClient(const NWMEndpoint &ep);
		std::vector<std::shared_ptr<NWMServerClient>> m_clients;
		std::vector<std::shared_ptr<NWMServerClient>> m_clientsIncoming;
		std::vector<boost::asio::ip::address> m_banned;
		std::vector<std::function<bool(void)>> m_callQueue;
		std::mutex m_clientMutex;
		ChronoTimePoint m_lastCheck;
		std::function<void(NWMServerClient *, CLIENT_DROPPED)> m_cbClientDropped;
		std::function<void(NWMServerClient *)> m_cbClient;

		NWMServer(const std::shared_ptr<SVNWMUDPConnection> &udp, const std::shared_ptr<SVNWMTCPConnection> &tcp);
		// Same as other HandlePacket overload, but is only called if the message is associated with a valid session / client
		// Called on thead!
		virtual bool HandlePacket(NWMSession *session, NWMServerClient *cl, unsigned int id, NetPacket &packet);
		// Session and client might be nullptr if this is a one-time request
		// Called on thead!
		virtual bool HandlePacket(const NWMEndpoint &ep, NWMSession *session, NWMServerClient *cl, unsigned int id, NetPacket &packet);

		// Called before HandlePacket is called
		virtual void OnPacketReceived(const NWMEndpoint &ep, NWMSession *session, NWMServerClient *cl, unsigned int id, NetPacket &packet);

		void OnClientPacketSent(NWMServerClient *cl, const NetPacket &packet);

		virtual std::shared_ptr<NWMServerClient> CreateClient() = 0;
		virtual void OnClientConnected(NWMServerClient *cl) = 0;
		virtual void OnClientDropped(NWMServerClient *cl, CLIENT_DROPPED reason) = 0;
		std::vector<SessionHandle> GetAllClients();
		void GetAllClients(std::vector<SessionHandle> &clients);
		template<class T>
		std::shared_ptr<T> CreateClient();
		void DropClient(NWMServerClient *client, CLIENT_DROPPED reason, bool bDontTellClient = false, bool bLock = true);
		void ScheduleDropClient(NWMServerClient *client, CLIENT_DROPPED reason);

		std::vector<NWMServerClient *> GetFilterTargets(const RecipientFilter &filter);
	  public:
		template<class T>
		static std::unique_ptr<T> Create(uint16_t tcpPort, uint16_t udpPort, unsigned char conType = NETWORK_CON_TYPE_UDP);
		template<class T>
		static std::unique_ptr<T> Create(uint16_t port, unsigned char conType = NETWORK_CON_TYPE_UDP);
		//static NWMServer *Create(unsigned int masterServerPort,unsigned char conType=NETWORK_CON_TYPE_UDP);
		virtual ~NWMServer() override;
		void BanIP(const std::string &ip);
		void BanIPs(const std::vector<std::string> &ips);
		bool IsIPBanned(const std::string &ip);
		bool IsIPBanned(const boost::asio::ip::address &ip);
		std::vector<SessionHandle> GetClients();
		void GetClients(std::vector<SessionHandle> &clients);
		SessionHandle GetClient(const std::string &ip, unsigned short port);
		SessionHandle GetClient(const boost::asio::ip::address &address, unsigned short port);
		void BroadcastTCP(const NetPacket &packet);
		void BroadcastUDP(const NetPacket &packet);
		void SendPacketTCP(const NetPacket &packet, const RecipientFilter &rp);
		void SendPacketUDP(const NetPacket &packet, const RecipientFilter &rp);
		virtual void Run() override;
		void Shutdown();
		void SetClientDroppedHandle(const std::function<void(NWMServerClient *, CLIENT_DROPPED)> &cbClientDropped);
		void SetClientHandle(const std::function<void(NWMServerClient *)> &cbClient);
		int GetClientCount() const;
		int GetMaxClients() const;
		void SetMaxClients(int numClients);
		bool AcceptClient(NWMSession *session);
	};

	template<class T>
	std::shared_ptr<T> NWMServer::CreateClient()
	{
		return std::shared_ptr<T>(new T(this));
	}

	template<class T>
	std::unique_ptr<T> NWMServer::Create(uint16_t tcpPort, uint16_t udpPort, unsigned char conType)
	{
		std::shared_ptr<SVNWMUDPConnection> udp;
		if(conType == NETWORK_CON_TYPE_UDP || conType == NETWORK_CON_TYPE_BOTH) {
			udp = std::shared_ptr<SVNWMUDPConnection>(new SVNWMUDPConnection(udpPort));
			udp->Initialize();
		}
		std::shared_ptr<SVNWMTCPConnection> tcp;
		if(conType == NETWORK_CON_TYPE_TCP || conType == NETWORK_CON_TYPE_BOTH) {
			tcp = std::shared_ptr<SVNWMTCPConnection>(new SVNWMTCPConnection(tcpPort));
			tcp->Initialize();
		}
		return std::unique_ptr<T>(new T(udp, tcp));
	}

	template<class T>
	std::unique_ptr<T> NWMServer::Create(unsigned short port, unsigned char conType)
	{
		return Create<T>(port, port, conType);
	}
}
