// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.network_manager;

import :udp_connection;

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
