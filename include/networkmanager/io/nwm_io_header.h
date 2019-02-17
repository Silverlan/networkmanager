/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_IO_HEADER__
#define __NWM_IO_HEADER__

#include <memory>
#include <stdint.h>

class NWMIOHeader
{
protected:
	std::unique_ptr<uint8_t[]> m_header;
public:
	NWMIOHeader();
	virtual ~NWMIOHeader();
};

#endif