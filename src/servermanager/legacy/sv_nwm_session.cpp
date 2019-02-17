/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "servermanager/legacy/sv_nwm_session.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
DEFINE_BASE_HANDLE(,NWMSession,NWMSession);

NWMSession::NWMSession()
	: m_handle(this)
{}

NWMSession::~NWMSession()
{
	m_handle.Invalidate();
}

void NWMSession::Release()
{
	m_handle.ptr = nullptr;
}

void NWMSession::Initialize()
{
	InitializeSharedPtr();
}

bool NWMSession::IsTCP() const {return !IsUDP();}

bool NWMSession::IsTarget(const NWMEndpoint &ep)
{
	auto &epOther = GetRemoteEndPoint();
	return (epOther == ep) ? true : false;
}
bool NWMSession::IsTarget(const boost::asio::ip::address &address,unsigned short port)
{
	auto &ep = GetRemoteEndPoint();
	return (ep == address && ep.GetPort() == port) ? true : false;
}

NWMSessionHandle NWMSession::GetHandle() {return m_handle;}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
