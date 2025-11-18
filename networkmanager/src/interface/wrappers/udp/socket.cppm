// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.network_manager:udp.socket;

export import :udp.core;
export import :udp.endpoint;
import :unique_void;

export namespace nwm {
	class IOService;
	// Note: Can't use TBoostWrapper as base since boost tcp socket cannot be pre-declared
	class UDPSocket {
	  public:
		UDPSocket(nwm::IOService &ioService);
		UDPSocket(nwm::IOService &ioService, const nwm::UDP &udp);
		UDPSocket(nwm::IOService &ioService, const nwm::UDPEndpoint &udp);

		const void *operator*() const;
		void *operator*();
		void *GetBoostObject();
	  private:
		impl::unique_void_ptr m_socket;
	};
};
