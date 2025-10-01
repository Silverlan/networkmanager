// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.network_manager;

import :error_code;

nwm::ErrorCode::ErrorCode(const ErrorCode &other) : TBoostWrapperCopyable<boost::system::error_code> {other} {}
nwm::ErrorCode::operator bool() const { return static_cast<bool>(GetBoostObject()); }
bool nwm::ErrorCode::operator!() const { return !static_cast<bool>(*this); }
std::string nwm::ErrorCode::Message() const { return GetBoostObject().message(); }

nwm::ErrorCode &nwm::ErrorCode::operator=(const ErrorCode &other)
{
	TBoostWrapperCopyable<boost::system::error_code>::operator=(other);
	return *this;
}

nwm::ErrorCode &nwm::ErrorCode::operator=(const boost::system::error_code &o)
{
	TBoostWrapperCopyable<boost::system::error_code>::operator=(o);
	return *this;
}
