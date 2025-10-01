// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>
#include <string>
#include <stdexcept>

export module pragma.network_manager:error_handle;

export import :error_code;

export {
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
}
