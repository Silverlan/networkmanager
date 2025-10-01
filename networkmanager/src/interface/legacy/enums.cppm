// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <cinttypes>

export module pragma.network_manager:legacy_enums;

export {
	constexpr uint32_t NETWORK_CON_TYPE_UDP = 0;
	constexpr uint32_t NETWORK_CON_TYPE_TCP = 1;
	constexpr uint32_t NETWORK_CON_TYPE_BOTH = 2;

	constexpr uint32_t NETWORK_ERROR_UNHANDLED_PACKET = 10;
}
