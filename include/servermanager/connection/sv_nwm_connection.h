/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_CONNECTION_H__
#define __SV_NWM_CONNECTION_H__

#include "networkmanager/nwm_connection.h"
#include "servermanager/legacy/sv_nwm_session.h"
#include <functional>

namespace nwm {
	class Server;
};
class NWMUDPSession;
class NWMTCPSession;
class NWMSession;
class SVNWMConnection {
  public:
	friend nwm::Server;
	friend NWMUDPSession;
	friend NWMTCPSession;
  public:
	struct AcceptCallbacks {
		std::function<void(void)> lock = nullptr;
		std::function<bool(nwm::IPAddress, uint16_t)> has_client = nullptr;
		std::function<bool(NWMSession *)> accept_client = nullptr;
		std::function<void(void)> unlock = nullptr;
	};

	SVNWMConnection();
	virtual void Initialize() = 0;
	void SetClientHandle(const std::function<void(NWMSession *)> &cbClient);
	void SetAcceptCallbacks(const AcceptCallbacks &callbacks);
	virtual void Run();
	virtual void SetTimeoutDuration(double duration);
	virtual bool IsClosing() const = 0;
	virtual void Close() = 0;
	virtual void Terminate() = 0;
  protected:
	AcceptCallbacks m_acceptCallbacks;

	std::function<void(NWMSession *)> m_clientHandle;
	double m_tSessionTimeout;
	std::vector<NWMSessionHandle> m_sessions;
	virtual void OnClientConnected(NWMSession *session);
	NWMSession *GetSession(const nwm::UDPEndpoint &ep);
	virtual void Accept() = 0;
	NWMSessionHandle AddSession(const std::shared_ptr<NWMSession> &session);
	void RemoveSession(NWMSession *session);
	void AttemptTermination();
	virtual void ScheduleTermination() = 0;
};

#endif
