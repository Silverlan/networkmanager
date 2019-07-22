/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_IO_BASE_H__
#define __NWM_IO_BASE_H__

#include "networkmanager/nwm_endpoint.h"
#include "networkmanager/nwm_error_handle.h"
#include "networkmanager/nwm_event_base.h"
#include <sharedutils/netpacket.hpp>
#include "networkmanager/wrappers/nwm_error_code.hpp"
#include "networkmanager/wrappers/nwm_ip_address.hpp"
#include <chrono>

#define NWM_USE_IPV6 0

using ChronoTimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
using ChronoDuration = std::chrono::duration<std::chrono::high_resolution_clock::rep,std::chrono::high_resolution_clock::period>;

class NWMIOBase
	: public std::enable_shared_from_this<NWMIOBase>,
	public NWMEventBase
{
protected:
	std::function<void(void)> m_closeHandle;
	ChronoTimePoint m_tSent;
	ChronoTimePoint m_tLastMessage;
	nwm::ErrorCode m_lastError;
	double m_tTimeout;
	bool m_bTimeoutIfConnectionActive = true;
	bool m_bTimedOut;
	bool m_bClosing;
	bool m_bTerminating;
	bool m_bTerminated;
	NWMEndpoint m_remoteEndpoint;
	NWMEndpoint m_localEndpoint;
	bool UpdateTermination();
	void Close(bool bForce);
	virtual void ScheduleTermination();
	virtual void Terminate();
	virtual bool ShouldTerminate();
	virtual void OnTimedOut();
	template<class T>
		T *GetRemoteEndPoint();
	template<class S,class T>
		T *GetRemoteEndPoint();
	virtual void OnTerminated();
	virtual void OnError(const nwm::ErrorCode &err);
public:
	NWMIOBase();
	virtual ~NWMIOBase();
	virtual void SetCloseHandle(const std::function<void(void)> &cbClose);
	virtual bool IsConnectionActive();
	virtual void SetReady()=0;
	virtual bool IsReady() const=0;
	void Close();
	virtual bool IsClosing() const;
	virtual bool IsTerminated() const;
	virtual std::string GetIP() const=0;
	virtual unsigned short GetPort() const=0;
	virtual nwm::IPAddress GetAddress() const=0;
	virtual std::string GetLocalIP() const=0;
	virtual unsigned short GetLocalPort() const=0;
	virtual nwm::IPAddress GetLocalAddress() const=0;
	virtual const NWMEndpoint &GetRemoteEndPoint() const;
	virtual const NWMEndpoint &GetLocalEndPoint() const;
	virtual void Run();
	void SetTimeoutDuration(double duration,bool bIfConnectionActive);
	bool IsTimedOut() const;
	const nwm::ErrorCode &GetLastError() const;
};

template<class T>
	T *NWMIOBase::GetRemoteEndPoint()
{
	return static_cast<T*>(m_remoteEndpoint.get());
}

template<class S,class T>
	T *NWMIOBase::GetRemoteEndPoint()
{
	return static_cast<S*>(m_remoteEndpoint.get())->get();
}

#endif