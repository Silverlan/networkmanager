#include "networkmanager/wrappers/nwm_udp_resolver.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"
#include "networkmanager/nwm_boost.h"

nwm::UDPResolver::UDPResolver(nwm::IOService &ioService) : m_resolver {impl::unique_void<boost::asio::ip::udp::resolver>(new boost::asio::ip::udp::resolver {*ioService})} {}

const void *nwm::UDPResolver::operator*() const { return const_cast<UDPResolver *>(this)->operator*(); }
void *nwm::UDPResolver::operator*() { return GetBoostObject(); }
void *nwm::UDPResolver::GetBoostObject() { return m_resolver.get(); }
