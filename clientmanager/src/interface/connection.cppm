// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>

export module pragma.client_manager:connection;

export import pragma.network_manager;

export class CLNWMConnection {
  protected:
	CLNWMConnection(NWMConnection *con);
	NWMConnection *m_connection;
	bool m_bRegistered;
	bool m_bDisconnecting;
	std::function<void(void)> m_connectionHandle;
	std::function<void(nwm::ClientDropped)> m_disconnectedHandle;
	void OnConnected();
	virtual void Close() = 0;
	virtual void OnDisconnected(nwm::ClientDropped reason);
	virtual void OnTimedOut();
  public:
	void Disconnect();
	void SetConnectionHandle(const std::function<void(void)> cbConnection);
	void SetDisconnectionHandle(const std::function<void(nwm::ClientDropped)> cbDisconnected);
	virtual bool IsClosing() const = 0;
	virtual void SendPacket(const NetPacket &packet, bool bOwn = false) = 0;
	virtual void Run() = 0;
};
