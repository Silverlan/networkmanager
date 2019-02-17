/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_MANAGER_HPP__
#define __SV_NWM_MANAGER_HPP__

#include <networkmanager/interface/nwm_manager.hpp>
#include "servermanager/interface/sv_nwm_clienthandle.hpp"

#undef SendMessage

class SVNWMUDPConnection;
class SVNWMTCPConnection;
class NWMSession;
namespace nwm
{
	class ServerClient;
	class RecipientFilter
	{
	public:
		enum class Type : uint32_t
		{
			Include = 0,
			Exclude
		};

		RecipientFilter(Type type=Type::Include);
		void SetFilterType(Type type);
		Type GetFilterType() const;
		void Add(const ServerClientHandle &hSession);
		void Remove(const ServerClientHandle &hSession);
		void Add(ServerClient *client);
		void Remove(ServerClient *client);
		bool HasRecipient(const ServerClientHandle &hSession) const;
		bool HasRecipient(ServerClient *client) const;
		const std::vector<ServerClientHandle> &get() const;
		std::vector<ServerClientHandle> &get();
		size_t GetRecipientCount() const;
	private:
		std::vector<ServerClientHandle> m_sessions;
		Type m_type = Type::Include;
	};
	class Server
		: public impl::ManagerBase
	{
	public:
		template<class T>
			static std::unique_ptr<T> Create(uint16_t tcpPort,uint16_t udpPort,ConnectionType conType=ConnectionType::UDP);
		template<class T>
			static std::unique_ptr<T> Create(uint16_t port,ConnectionType conType=ConnectionType::UDP);
		enum class MessageIn : uint32_t
		{
			Ping = 1,
			ClientDisconnect = 4,

			ReceiveMessage = 11
		};
		enum class MessageOut : uint32_t
		{
			Pong = 51,
			RegisterConfirmation = 52,
			Dropped = 53,

			SendServerList = 3,
			SendMessage = 12
		};
		virtual ~Server() override;
		void BanIP(const std::string &ip);
		void BanIPs(const std::vector<std::string> &ips);
		bool IsIPBanned(const std::string &ip);
		bool IsIPBanned(const boost::asio::ip::address &ip);

		int32_t GetMaxClients() const;
		void SetMaxClients(int32_t numClients);
		int32_t GetClientCount() const;
		std::vector<ServerClientHandle> GetClients() const;
		void GetClients(std::vector<ServerClientHandle> &clients) const;

		void BroadcastPacket(Protocol protocol,const NetPacket &packet);
		void SendPacket(Protocol protocol,const NetPacket &packet,const RecipientFilter &rp);
		void DropClient(ServerClient *client,ClientDropped reason,bool bDontTellClient=false);
		void SetClientDroppedHandle(const std::function<void(ServerClient*,ClientDropped)> &cbClientDropped);
		void SetClientHandle(const std::function<void(ServerClient*)> &cbClient);
		void Shutdown();

		virtual std::shared_ptr<ServerClient> CreateClient()=0;
		virtual void OnClientDropped(ServerClient *cl,ClientDropped reason)=0;
		virtual void OnClientConnected(ServerClient *cl)=0;

		std::vector<ServerClientHandle> GetAllClients() const;
		void GetAllClients(std::vector<ServerClientHandle> &clients) const;
		ServerClient *FindClient(const NWMEndpoint &ep);
		template<class T>
			std::shared_ptr<T> CreateClient();
	protected:
		Server(const std::shared_ptr<SVNWMUDPConnection> &udp,const std::shared_ptr<SVNWMTCPConnection> &tcp);

		ServerClientHandle GetClient(const std::string &ip,uint16_t port);
		ServerClientHandle GetClient(const boost::asio::ip::address &address,uint16_t port);
		std::vector<ServerClient*> GetFilterTargets(const RecipientFilter &filter);
		void OnClientPacketSent(ServerClient *cl,const NetPacket &packet);

		// Same as other HandlePacket overload, but is only called if the message is associated with a valid session / client
		virtual bool HandlePacket(ServerClient *cl,uint32_t id,NetPacket &packet);
		// Session and client might be nullptr if this is a one-time request
		virtual bool HandlePacket(const NWMEndpoint &ep,ServerClient *cl,uint32_t id,NetPacket &packet);

		// Called immediately after the packet has been received on the network thread, handle with care!
		virtual bool HandleAsyncPacket(const NWMEndpoint &ep,NWMSession *session,uint32_t id,NetPacket &packet);

		// Called before HandlePacket is called
		virtual void OnPacketReceived(const NWMEndpoint &ep,ServerClient *cl,uint32_t id,NetPacket &packet);

		std::vector<ServerClientHandle> GetAllClients(bool bLockClients) const;
		void GetAllClients(std::vector<ServerClientHandle> &clients,bool bLockClients) const;

		// NOT thread-safe!
		// 'm_clientMutex' has to be locked for this function! 
		bool AcceptClient(NWMSession *session);
		virtual void Run() override;
	private:
		friend ServerClient;
		void RemoveClient(std::vector<std::shared_ptr<ServerClient>> &clients,ServerClient *cl);

		std::atomic<int32_t> m_maxClients = {-1};
		std::vector<boost::asio::ip::address> m_banned;
		mutable std::mutex m_banListMutex;
		mutable std::mutex m_clientMutex; // Not required for actual clients; Only required for 'm_clients' and 'm_clientsConnecting' containers!

		std::function<void(ServerClient*,ClientDropped)> m_cbClientDropped;
		std::function<void(ServerClient*)> m_cbClient;

		// 'm_clients' and 'm_clientsConnecting' must only be manipulated on main thread!
		// Fully connected clients
		std::vector<std::shared_ptr<ServerClient>> m_clients;

		// Clients which are still connecting
		std::vector<std::shared_ptr<ServerClient>> m_clientsConnecting;

		// NOT thread-safe!
		void UpdateClientReadyState(ServerClient *cl);
	};
};

#endif
