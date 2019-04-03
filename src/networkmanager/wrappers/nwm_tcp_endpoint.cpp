#include "networkmanager/wrappers/nwm_tcp_endpoint.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"
#include "networkmanager/nwm_boost.h"

nwm::TCPEndpoint::TCPEndpoint(const TCPEndpoint &ep)
	: m_endPoint{impl::unique_void<boost::asio::ip::tcp::endpoint>(new boost::asio::ip::tcp::endpoint{*static_cast<const boost::asio::ip::tcp::endpoint*>(ep.GetBoostObject())})}
{}
nwm::TCPEndpoint::TCPEndpoint(void *boostTCPEndpoint)
	: m_endPoint{impl::unique_void<boost::asio::ip::tcp::endpoint>(new boost::asio::ip::tcp::endpoint{*static_cast<boost::asio::ip::tcp::endpoint*>(boostTCPEndpoint)})}
{}
nwm::TCPEndpoint::TCPEndpoint()
	: m_endPoint{impl::unique_void<boost::asio::ip::tcp::endpoint>(new boost::asio::ip::tcp::endpoint{})}
{}

const void *nwm::TCPEndpoint::operator*() const {return const_cast<TCPEndpoint*>(this)->operator*();}
void *nwm::TCPEndpoint::operator*() {return GetBoostObject();}
const void *nwm::TCPEndpoint::GetBoostObject() const {return const_cast<TCPEndpoint*>(this)->GetBoostObject();}
void *nwm::TCPEndpoint::GetBoostObject() {return m_endPoint.get();}
