// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_IO_HEADER__
#define __NWM_IO_HEADER__

#include <memory>
#include <stdint.h>

class NWMIOHeader {
  protected:
	std::unique_ptr<uint8_t[]> m_header;
  public:
	NWMIOHeader();
	virtual ~NWMIOHeader();
};

#endif
