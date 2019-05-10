/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/udp_handler/udp_message_base.h"
#include "networkmanager/nwm_boost.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
UDPMessageBase::UDPMessageBase()
	: m_ioService{}
{}

UDPMessageBase::~UDPMessageBase()
{
	m_ioService->stop();
}

void UDPMessageBase::Poll()
{
	m_ioService->reset();
	m_ioService->poll();
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
