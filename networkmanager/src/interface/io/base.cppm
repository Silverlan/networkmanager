// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <sharedutils/util_clock.hpp>
#include <chrono>

export module pragma.network_manager:io.base;

export import :endpoint;
export import :enums;
export import :error_code;
export import :event_base;

#define NWM_USE_IPV6 0

export {
	class NWMIOBase : public std::enable_shared_from_this<NWMIOBase>, public NWMEventBase {
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
		template<class S, class T>
		T *GetRemoteEndPoint();
		virtual void OnTerminated();
		virtual void OnError(const nwm::ErrorCode &err);
	public:
		NWMIOBase();
		virtual ~NWMIOBase();
		virtual void SetCloseHandle(const std::function<void(void)> &cbClose);
		virtual bool IsConnectionActive();
		virtual void SetReady() = 0;
		virtual bool IsReady() const = 0;
		void Close();
		virtual bool IsClosing() const;
		virtual bool IsTerminated() const;
		virtual std::string GetIP() const = 0;
		virtual unsigned short GetPort() const = 0;
		virtual nwm::IPAddress GetAddress() const = 0;
		virtual std::string GetLocalIP() const = 0;
		virtual unsigned short GetLocalPort() const = 0;
		virtual nwm::IPAddress GetLocalAddress() const = 0;
		virtual const NWMEndpoint &GetRemoteEndPoint() const;
		virtual const NWMEndpoint &GetLocalEndPoint() const;
		virtual void Run();
		void SetTimeoutDuration(double duration, bool bIfConnectionActive);
		bool IsTimedOut() const;
		const nwm::ErrorCode &GetLastError() const;
	};

	template<class T>
	T *NWMIOBase::GetRemoteEndPoint()
	{
		return static_cast<T *>(m_remoteEndpoint.get());
	}

	template<class S, class T>
	T *NWMIOBase::GetRemoteEndPoint()
	{
		return static_cast<S *>(m_remoteEndpoint.get())->get();
	}
}
