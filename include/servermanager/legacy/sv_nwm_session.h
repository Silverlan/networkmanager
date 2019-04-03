/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_SESSION_H__
#define __SV_NWM_SESSION_H__

#include "networkmanager/nwm_endpoint.h"
#include <sharedutils/def_handle.h>
#include <functional>

class NWMSession;
DECLARE_BASE_HANDLE_EXT(,NWMSession,NWMSession,
public:
	std::shared_ptr<NWMSession> ptr;
);

class NetPacket;
class NWMIOBase;
class NWMSession
{
protected:
	NWMSession();
	NWMSessionHandle m_handle;
	virtual void OnPacketReceived()=0;
	virtual void InitializeSharedPtr()=0;
public:
	virtual ~NWMSession();
	void Initialize();
	virtual bool IsUDP() const=0;
	bool IsTCP() const;
	virtual void Release();
	virtual const NWMEndpoint &GetRemoteEndPoint() const=0;
	virtual void SetPacketHandle(const std::function<void(const NWMEndpoint&,NWMIOBase*,unsigned int,NetPacket&)> &cbPacket)=0;
	virtual void Close()=0;
	virtual void Run()=0;
	virtual void SendPacket(const NetPacket &packet)=0;
	virtual std::string GetIP() const=0;
	virtual unsigned short GetPort() const=0;
	virtual nwm::IPAddress GetAddress() const=0;
	bool IsTarget(const NWMEndpoint &ep);
	bool IsTarget(const nwm::IPAddress &address,unsigned short port);
	virtual void SetTimeoutDuration(double duration)=0;
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose)=0;
	NWMSessionHandle GetHandle();
};

#endif