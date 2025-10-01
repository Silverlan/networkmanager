// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.network_manager:tcp.endpoint;

import :unique_void;

export namespace nwm {
	class IOService;
	// Note: Can't use TBoostWrapper as base since boost tcp endpoint cannot be pre-declared
	class TCPEndpoint {
	  public:
		TCPEndpoint(const TCPEndpoint &ep);
		TCPEndpoint(void *boostTCPEndpoint);
		TCPEndpoint();

		const void *operator*() const;
		void *operator*();
		const void *GetBoostObject() const;
		void *GetBoostObject();
	  private:
		impl::unique_void_ptr m_endPoint;
	};
};
