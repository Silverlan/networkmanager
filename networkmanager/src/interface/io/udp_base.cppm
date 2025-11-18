// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.network_manager:io.udp_base;

export import :endpoint;
export import :error_code;
export import :mutable_buffer;
export import :udp.socket;

export class NWMUDPIOBase {
  protected:
	virtual void AsyncWrite(nwm::UDPSocket *socket, const std::vector<nwm::MutableBuffer> &buffers, const NWMEndpoint &endPoint, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f);
	virtual void AsyncRead(nwm::UDPSocket *socket, const std::vector<nwm::MutableBuffer> &buffers, const NWMEndpoint &endPoint, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f, bool bPeek = false);
  public:
	NWMUDPIOBase();
	virtual ~NWMUDPIOBase();
	virtual bool IsClosing() const = 0;
	virtual std::string GetIP() const = 0;
	virtual unsigned short GetPort() const = 0;
	virtual void Terminate();
	virtual bool IsTerminated() const;
};
