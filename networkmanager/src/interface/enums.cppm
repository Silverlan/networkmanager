// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <memory>
#include <string>
#include "sharedutils/util_clock.hpp"

export module pragma.network_manager:enums;

export {
	// Number of messages reserved for internal use. Don't use package IDs below this value!
	constexpr uint32_t NWM_MESSAGE_RESERVED = 100;
	//#define NETWORK_ERROR_UNHANDLED_PACKET 10
	using ChronoTimePoint = util::Clock::time_point;
	using ChronoDuration = util::Clock::duration;
	namespace nwm {
		enum class ConnectionType : uint32_t { UDP = 0, TCP, TCPUDP };
		enum class Protocol : uint32_t { UDP = 0, TCP };
		enum class ClientDropped : int8_t { Disconnected = 0, Timeout, Kicked, Shutdown, Error };
		enum class Result : uint32_t { UnhandledPacket = 10 };
		std::string client_dropped_enum_to_string(ClientDropped e);
		std::string protocol_enum_to_string(Protocol e);
	};

}
