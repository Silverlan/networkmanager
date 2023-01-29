/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_TCPCONNECTION_H__
#define __NWM_TCPCONNECTION_H__

#include <vector>
#include "nwm_connection.h"

class NWMTCPConnection : public NWMConnection {
  protected:
	virtual void CloseSocket() override;
  public:
	NWMTCPConnection();
	virtual ~NWMTCPConnection() override;
	virtual void Close() override;
	virtual void Terminate() override;
	virtual void SetNagleAlgorithmEnabled(bool b) = 0;
};

#endif
