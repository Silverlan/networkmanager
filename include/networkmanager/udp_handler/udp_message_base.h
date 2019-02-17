/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __UDP_MESSAGE_BASE_H__
#define __UDP_MESSAGE_BASE_H__

#include "networkmanager/nwm_boost.h"

class UDPMessageBase
{
protected:
	UDPMessageBase();
	virtual ~UDPMessageBase();
	boost::asio::io_service m_ioService;
public:
	virtual void Poll();
};

#endif