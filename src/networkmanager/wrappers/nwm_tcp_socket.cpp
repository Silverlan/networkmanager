#include "networkmanager/wrappers/nwm_tcp_socket.hpp"
#include "networkmanager/wrappers/nwm_io_service.hpp"
#include "networkmanager/nwm_boost.h"

nwm::TCPSocket::TCPSocket(nwm::IOService &ioService)
	: m_socket{impl::unique_void<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket{*ioService})}
{}

const void *nwm::TCPSocket::operator*() const {return const_cast<TCPSocket*>(this)->operator*();}
void *nwm::TCPSocket::operator*() {return GetBoostObject();}
void *nwm::TCPSocket::GetBoostObject() {return m_socket.get();}
