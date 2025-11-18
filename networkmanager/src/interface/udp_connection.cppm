// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.network_manager:udp_connection;

export import :connection;

export class NWMUDPConnection : public NWMConnection {
  protected:
	virtual void CloseSocket() override = 0;
  public:
	NWMUDPConnection();
	virtual ~NWMUDPConnection() override;
	virtual void Close() override;
	virtual void Terminate() override;
};
