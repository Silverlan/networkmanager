#include "networkmanager/wrappers/nwm_udp_endpoint.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"
#include "networkmanager/nwm_boost.h"

nwm::UDPEndpoint::UDPEndpoint(const UDPEndpoint &ep) : m_endPoint {impl::unique_void<boost::asio::ip::udp::endpoint>(new boost::asio::ip::udp::endpoint {*static_cast<const boost::asio::ip::udp::endpoint *>(ep.GetBoostObject())})} {}
nwm::UDPEndpoint::UDPEndpoint(void *boostUDPEndpoint) : m_endPoint {impl::unique_void<boost::asio::ip::udp::endpoint>(new boost::asio::ip::udp::endpoint {*static_cast<boost::asio::ip::udp::endpoint *>(boostUDPEndpoint)})} {}
nwm::UDPEndpoint::UDPEndpoint() : m_endPoint {impl::unique_void<boost::asio::ip::udp::endpoint>(new boost::asio::ip::udp::endpoint {})} {}
nwm::UDPEndpoint &nwm::UDPEndpoint::operator=(const UDPEndpoint &other)
{
	m_endPoint = impl::unique_void<boost::asio::ip::udp::endpoint>(new boost::asio::ip::udp::endpoint {*static_cast<const boost::asio::ip::udp::endpoint *>(other.GetBoostObject())});
	return *this;
}

const void *nwm::UDPEndpoint::operator*() const { return const_cast<UDPEndpoint *>(this)->operator*(); }
void *nwm::UDPEndpoint::operator*() { return GetBoostObject(); }
const void *nwm::UDPEndpoint::GetBoostObject() const { return const_cast<UDPEndpoint *>(this)->GetBoostObject(); }
void *nwm::UDPEndpoint::GetBoostObject() { return m_endPoint.get(); }
