/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_ERROR_HANDLE_H__
#define __NWM_ERROR_HANDLE_H__

#include <functional>
#include <string>
#include <stdexcept>
#include "networkmanager/wrappers/nwm_error_code.hpp"

namespace nwm {
	std::string get_error_name(int id);
};

class NWMException : public std::runtime_error {
  public:
	NWMException(const std::string &err);
};

class NWMErrorHandle {
  protected:
	NWMErrorHandle();
	std::function<void(const nwm::ErrorCode &)> m_errorHandle;
	virtual bool HandleError(const nwm::ErrorCode &error);
  public:
	static std::string GetErrorName(int id);
	void SetErrorHandle(const std::function<void(const nwm::ErrorCode &)> &cbError);
};

#endif
