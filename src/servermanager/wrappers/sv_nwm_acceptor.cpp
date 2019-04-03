#include "servermanager/wrappers/sv_nwm_acceptor.hpp"
#include <networkmanager/wrappers/nwm_impl_boost.hpp>
#include <networkmanager/wrappers/nwm_io_service.hpp>
#include <networkmanager/wrappers/nwm_tcp_endpoint.hpp>
#include <networkmanager/nwm_boost.h>

nwm::TCPAcceptor::TCPAcceptor(nwm::IOService &ioService,const TCPEndpoint &ep)
	: m_acceptor{impl::unique_void<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor{*ioService,*cast_endpoint(ep)})}
{}

const void *nwm::TCPAcceptor::operator*() const {return const_cast<TCPAcceptor*>(this)->operator*();}
void *nwm::TCPAcceptor::operator*() {return GetBoostObject();}
const void *nwm::TCPAcceptor::GetBoostObject() const {return const_cast<TCPAcceptor*>(this)->GetBoostObject();}
void *nwm::TCPAcceptor::GetBoostObject() {return m_acceptor.get();}
