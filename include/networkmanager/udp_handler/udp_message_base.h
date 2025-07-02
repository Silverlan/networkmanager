// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __UDP_MESSAGE_BASE_H__
#define __UDP_MESSAGE_BASE_H__

#include "networkmanager/wrappers/nwm_io_service.hpp"

class UDPMessageBase {
  protected:
	UDPMessageBase();
	virtual ~UDPMessageBase();
	nwm::IOService m_ioService;
  public:
	virtual void Poll();
};

#endif
