/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/io/nwm_io_header.h"
#include <sharedutils/netpacket.hpp>

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMIOHeader::NWMIOHeader() { m_header = std::unique_ptr<uint8_t[]>(new uint8_t[NWM_PACKET_HEADER_EXTENDED_SIZE]); }

NWMIOHeader::~NWMIOHeader() { m_header = nullptr; }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
