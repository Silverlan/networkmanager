/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_UDPCONNECTION_H__
#define __NWM_UDPCONNECTION_H__

#include "nwm_connection.h"

class NWMUDPConnection
	: public NWMConnection
{
protected:
	virtual void CloseSocket() override=0;
public:
	NWMUDPConnection();
	virtual ~NWMUDPConnection() override;
	virtual void Close() override;
	virtual void Terminate() override;
};

#endif