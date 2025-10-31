// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.network_manager:udp.resolver_query;

import :unique_void;

export namespace nwm {
	class IOService;
	// Note: Can't use TBoostWrapper as base since boost tcp socket cannot be pre-declared
	class UDPResolverQuery {
	  public:
		UDPResolverQuery(const std::string &ip, const std::string &port);

		const void *operator*() const;
		void *operator*();
		void *GetBoostObject();
	  private:
		impl::unique_void_ptr m_resolverQuery;
	};
};
