// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_UDPCONNECTION_H__
#define __NWM_UDPCONNECTION_H__

#include "nwm_connection.h"

class NWMUDPConnection : public NWMConnection {
  protected:
	virtual void CloseSocket() override = 0;
  public:
	NWMUDPConnection();
	virtual ~NWMUDPConnection() override;
	virtual void Close() override;
	virtual void Terminate() override;
};

#endif
