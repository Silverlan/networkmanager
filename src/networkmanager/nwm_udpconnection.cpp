/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
