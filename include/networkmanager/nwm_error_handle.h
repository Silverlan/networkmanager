// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

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
