// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string>

module pragma.network_manager;

import :enums;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif

std::string nwm::client_dropped_enum_to_string(ClientDropped e)
{
	switch(e) {
	case ClientDropped::Disconnected:
		return "disconnected";
	case ClientDropped::Timeout:
		return "timed out";
	case ClientDropped::Kicked:
		return "kicked";
	case ClientDropped::Shutdown:
		return "server shut down";
	case ClientDropped::Error:
		return "error";
	}
	return "unknown";
}

std::string nwm::protocol_enum_to_string(Protocol e)
{
	switch(e) {
	case Protocol::TCP:
		return "TCP";
	case Protocol::UDP:
		return "UDP";
	}
	return "unknown";
}

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
