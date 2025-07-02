// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

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
