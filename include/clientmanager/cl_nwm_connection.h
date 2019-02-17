/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __CL_NWM_CONNECTION_H__
#define __CL_NWM_CONNECTION_H__

#include "networkmanager/nwm_connection.h"
#include <functional>

#define NWM_MESSAGE_OUT_PING 1
#define NWM_MESSAGE_OUT_REGISTER_UDP 2
#define NWM_MESSAGE_OUT_REGISTER_TCP 3
#define NWM_MESSAGE_OUT_CLIENT_DISCONNECT 4

#define NWM_MESSAGE_IN_PONG 51
#define NWM_MESSAGE_IN_REGISTER_CONFIRMATION 52
#define NWM_MESSAGE_IN_DROPPED 53

namespace nwm {enum class ClientDropped : int8_t;};
class CLNWMConnection
{
protected:
	CLNWMConnection(NWMConnection *con);
	NWMConnection *m_connection;
	bool m_bRegistered;
	bool m_bDisconnecting;
	std::function<void(void)> m_connectionHandle;
	std::function<void(nwm::ClientDropped)> m_disconnectedHandle;
	void OnConnected();
	virtual void Close()=0;
	virtual void OnDisconnected(nwm::ClientDropped reason);
	virtual void OnTimedOut();
public:
	void Disconnect();
	void SetConnectionHandle(const std::function<void(void)> cbConnection);
	void SetDisconnectionHandle(const std::function<void(nwm::ClientDropped)> cbDisconnected);
	virtual bool IsClosing() const=0;
	virtual void SendPacket(const NetPacket &packet,bool bOwn=false)=0;
	virtual void Run()=0;
};

#endif