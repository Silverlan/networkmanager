// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.network_manager;

import :tcp_connection;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMTCPConnection::NWMTCPConnection() : NWMConnection() {}
NWMTCPConnection::~NWMTCPConnection() {}

void NWMTCPConnection::CloseSocket() {}

void NWMTCPConnection::Close() {}

void NWMTCPConnection::Terminate() { NWMConnection::Terminate(); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
