#ifndef __NWM_ERROR_CODE_HPP__
#define __NWM_ERROR_CODE_HPP__

#include "networkmanager/wrappers/nwm_boost_wrapper.hpp"
#include <string>

namespace boost {
	namespace system {
		class error_code;
	};
};
namespace nwm {
	class ErrorCode : public TBoostWrapperCopyable<boost::system::error_code> {
	  public:
		using TBoostWrapperCopyable<boost::system::error_code>::TBoostWrapperCopyable;
		ErrorCode(const ErrorCode &other);
		operator bool() const;
		bool operator!() const;
		std::string Message() const;

		// This is required to shut gcc up about some
		// weird linking issues
		ErrorCode &operator=(const ErrorCode &other);
		ErrorCode &operator=(const boost::system::error_code &o);
	};
};

#endif
