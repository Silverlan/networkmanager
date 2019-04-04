#include "networkmanager/wrappers/nwm_ip_address.hpp"
#include "networkmanager/wrappers/nwm_impl_boost.hpp"

bool nwm::IPAddress::operator==(const IPAddress &other) const {return **this == *other;}
bool nwm::IPAddress::operator!=(const IPAddress &other) const {return !operator==(other);}

std::string nwm::IPAddress::ToString() const
{
	return GetBoostObject().to_string();
}
