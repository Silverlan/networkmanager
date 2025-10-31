// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.network_manager:io.header;

export import std.compat;

export class NWMIOHeader {
  protected:
	std::unique_ptr<uint8_t[]> m_header;
  public:
	NWMIOHeader();
	virtual ~NWMIOHeader();
};
