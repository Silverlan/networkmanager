// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "clientmanager/cl_nwm_connection.h"
#include <networkmanager/interface/nwm_manager.hpp>

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
CLNWMConnection::CLNWMConnection(NWMConnection *con) : m_connection(con), m_bRegistered(false), m_bDisconnecting(false) {}

void CLNWMConnection::OnConnected()
{
	if(m_connectionHandle != nullptr)
		m_connectionHandle();
}
void CLNWMConnection::OnDisconnected(nwm::ClientDropped reason)
{
	if(m_disconnectedHandle != nullptr)
		m_disconnectedHandle(reason);
}

void CLNWMConnection::SetConnectionHandle(const std::function<void(void)> cbConnection) { m_connectionHandle = cbConnection; }
void CLNWMConnection::SetDisconnectionHandle(const std::function<void(nwm::ClientDropped)> cbDisconnected) { m_disconnectedHandle = cbDisconnected; }

void CLNWMConnection::Disconnect()
{
	if(m_bDisconnecting == true || m_connection->IsClosing())
		return;
	NetPacket out(NWM_MESSAGE_OUT_CLIENT_DISCONNECT);
	SendPacket(out, true);
	Close();
	OnDisconnected(nwm::ClientDropped::Disconnected);
	m_bDisconnecting = true;
}

void CLNWMConnection::OnTimedOut() { OnDisconnected(nwm::ClientDropped::Timeout); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
