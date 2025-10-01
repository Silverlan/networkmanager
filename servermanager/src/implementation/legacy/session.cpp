// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "sharedutils/def_handle.h"

module pragma.server_manager;

import :legacy.session;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
DEFINE_BASE_HANDLE(, NWMSession, NWMSession);

NWMSession::NWMSession() : m_handle(this) {}

NWMSession::~NWMSession() { m_handle.Invalidate(); }

void NWMSession::Release() { m_handle.ptr = nullptr; }

void NWMSession::Initialize() { InitializeSharedPtr(); }

bool NWMSession::IsTCP() const { return !IsUDP(); }

bool NWMSession::IsTarget(const NWMEndpoint &ep)
{
	auto &epOther = GetRemoteEndPoint();
	return (epOther == ep) ? true : false;
}
bool NWMSession::IsTarget(const nwm::IPAddress &address, unsigned short port)
{
	auto &ep = GetRemoteEndPoint();
	return (ep == address && ep.GetPort() == port) ? true : false;
}

NWMSessionHandle NWMSession::GetHandle() { return m_handle; }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
