// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.client_manager:enums;

export import std.compat;

export {
	uint32_t NWM_MESSAGE_OUT_PING = 1;
	uint32_t NWM_MESSAGE_OUT_REGISTER_UDP = 2;
	uint32_t NWM_MESSAGE_OUT_REGISTER_TCP = 3;
	uint32_t NWM_MESSAGE_OUT_CLIENT_DISCONNECT = 4;

	uint32_t NWM_MESSAGE_IN_PONG = 51;
	uint32_t NWM_MESSAGE_IN_REGISTER_CONFIRMATION = 52;
	uint32_t NWM_MESSAGE_IN_DROPPED = 53;
}
