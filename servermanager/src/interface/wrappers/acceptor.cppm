// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.server_manager:acceptor;

export import pragma.network_manager;

export namespace nwm {
	// Note: Can't use TBoostWrapper as base since boost acceptor cannot be pre-declared
	class TCPAcceptor {
	  public:
		TCPAcceptor(nwm::IOService &ioService, const TCPEndpoint &ep);

		const void *operator*() const;
		void *operator*();
		const void *GetBoostObject() const;
		void *GetBoostObject();
	  private:
		impl::unique_void_ptr m_acceptor;
	};
};
