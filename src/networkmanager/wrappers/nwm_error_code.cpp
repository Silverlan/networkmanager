#include "networkmanager/wrappers/nwm_error_code.hpp"
#include "networkmanager/wrappers/nwm_impl_boost.hpp"
#include "networkmanager/wrappers/nwm_boost_wrapper_impl.hpp"

nwm::ErrorCode::ErrorCode(const ErrorCode &other)
	: TBoostWrapperCopyable<boost::system::error_code>{other}
{}
nwm::ErrorCode::operator bool() const {return static_cast<bool>(GetBoostObject());}
bool nwm::ErrorCode::operator!() const {return !static_cast<bool>(*this);}
std::string nwm::ErrorCode::Message() const
{
	return GetBoostObject().message();
}
