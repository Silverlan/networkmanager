#include "networkmanager/wrappers/nwm_udp_resolver_query.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"
#include "networkmanager/nwm_boost.h"

nwm::UDPResolverQuery::UDPResolverQuery(const std::string &ip, const std::string &port) : m_resolverQuery {impl::unique_void<boost::asio::ip::udp::resolver::query>(new boost::asio::ip::udp::resolver::query {ip, port})} {}

const void *nwm::UDPResolverQuery::operator*() const { return const_cast<UDPResolverQuery *>(this)->operator*(); }
void *nwm::UDPResolverQuery::operator*() { return GetBoostObject(); }
void *nwm::UDPResolverQuery::GetBoostObject() { return m_resolverQuery.get(); }
