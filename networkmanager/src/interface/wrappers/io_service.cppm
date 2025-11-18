// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

export module pragma.network_manager:io_service;

export import :boost_wrapper;

export namespace nwm {
	class IOService : public TBoostWrapper<boost::asio::io_context> {
	  public:
		IOService();
	};
};
