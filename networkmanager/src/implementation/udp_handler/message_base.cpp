// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.network_manager;

import :udp_handler.core;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
UDPMessageBase::UDPMessageBase() : m_ioService {} {}

UDPMessageBase::~UDPMessageBase() { m_ioService->stop(); }

void UDPMessageBase::Poll()
{
	m_ioService->restart();
	m_ioService->poll();
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
