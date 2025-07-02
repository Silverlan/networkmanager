// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/wrappers/nwm_ip_address.hpp"
#include "networkmanager/wrappers/nwm_impl_boost.hpp"

bool nwm::IPAddress::operator==(const IPAddress &other) const { return **this == *other; }
bool nwm::IPAddress::operator!=(const IPAddress &other) const { return !operator==(other); }

nwm::IPAddress::IPAddress() : TBoostWrapperCopyable<boost::asio::ip::address> {boost::asio::ip::address {}} {}

std::string nwm::IPAddress::ToString() const { return GetBoostObject().to_string(); }
