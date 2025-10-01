// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.network_manager:udp_handler.core;

export import :io_service;

export class UDPMessageBase {
  protected:
	UDPMessageBase();
	virtual ~UDPMessageBase();
	nwm::IOService m_ioService;
  public:
	virtual void Poll();
};
