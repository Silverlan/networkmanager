/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/nwm_tcpconnection.h"
#include "clientmanager/legacy/cl_nwm_manager.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NWMTCPConnection::NWMTCPConnection()
	: NWMConnection()
{}
NWMTCPConnection::~NWMTCPConnection()
{}

void NWMTCPConnection::CloseSocket() {}

void NWMTCPConnection::Close() {}

void NWMTCPConnection::Terminate() {NWMConnection::Terminate();}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
