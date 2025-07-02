// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/nwm_udpconnection.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMUDPConnection::NWMUDPConnection() : NWMConnection() {}
NWMUDPConnection::~NWMUDPConnection() {}

void NWMUDPConnection::CloseSocket() {}

void NWMUDPConnection::Close() {}

void NWMUDPConnection::Terminate() { NWMConnection::Terminate(); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
