// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <memory>
#include <stdint.h>

export module pragma.network_manager:io.header;

export class NWMIOHeader {
  protected:
	std::unique_ptr<uint8_t[]> m_header;
  public:
	NWMIOHeader();
	virtual ~NWMIOHeader();
};
