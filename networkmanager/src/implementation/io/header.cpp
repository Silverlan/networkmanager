// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <cinttypes>
#include <memory>

module pragma.network_manager;

import :io.header;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMIOHeader::NWMIOHeader() { m_header = std::unique_ptr<uint8_t[]>(new uint8_t[NWM_PACKET_HEADER_EXTENDED_SIZE]); }

NWMIOHeader::~NWMIOHeader() { m_header = nullptr; }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
