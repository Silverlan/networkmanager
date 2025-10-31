// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.network_manager:tcp_connection;

export import :connection;

export class NWMTCPConnection : public NWMConnection {
  protected:
	virtual void CloseSocket() override;
  public:
	NWMTCPConnection();
	virtual ~NWMTCPConnection() override;
	virtual void Close() override;
	virtual void Terminate() override;
	virtual void SetNagleAlgorithmEnabled(bool b) = 0;
};
