/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/io/nwm_udp_io.h"
#include "networkmanager/nwm_error_handle.h"
#include "networkmanager/interface/nwm_manager.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NWMUDPIO::NWMUDPIO(boost::asio::io_service &ioService,unsigned short localPort)
	: NWMIO(),NWMUDPIOBase()
{
	try
	{
		udp::resolver resolver(ioService);
#if NWM_USE_IPV6 == 0
		udp::resolver::query query(udp::v4(),std::to_string(localPort));
#else
		udp::resolver::query query("::1",std::to_string(localPort));
#endif
		udp::endpoint localEndpoint = *resolver.resolve(query);
		m_localEndpoint = NWMEndpoint::CreateUDP(localEndpoint);
		m_socket = std::unique_ptr<udp::socket>(new udp::socket(ioService));
#if NWM_USE_IPV6 == 0
		m_socket->open(udp::v4());
#else
		m_socket->open(udp::v6());
#endif
		m_socket->bind(localEndpoint);
	}
	catch(std::exception &e)
	{
		m_socket = nullptr;
		throw NWMException(e.what());
	}
}

NWMUDPIO::~NWMUDPIO()
{
	CloseSocket();
	Terminate();
}

nwm::Protocol NWMUDPIO::GetProtocol() const {return nwm::Protocol::UDP;}
bool NWMUDPIO::IsTerminated() const {return NWMIO::IsTerminated() || NWMUDPIOBase::IsTerminated() || m_socket == nullptr || !m_socket->is_open();}

NWMEndpoint &NWMUDPIO::GetLocalEndPoint() {return m_localEndpoint;}

void NWMUDPIO::CloseSocket()
{
	if(m_socket == nullptr)
		return;
	if(m_socket->is_open())
	{
		//m_socket->shutdown(boost::asio::socket_base::shutdown_type::shutdown_both); // Not required for UDP sockets
		m_socket->close();
	}
	m_socket = nullptr;
}

void NWMUDPIO::Terminate()
{
	if(IsTerminated())
		return;
	NWMUDPIOBase::Terminate();
	CloseSocket();
	NWMIO::Terminate();
}

std::string NWMUDPIO::GetIP() const
{
	if(IsTerminated())
		return nwm::invalid_address();
	return nwm::to_string(m_socket->remote_endpoint());
}

boost::asio::ip::address NWMUDPIO::GetAddress() const
{
	if(IsTerminated())
		return boost::asio::ip::address();
	auto endPoint = m_socket->remote_endpoint();
	return endPoint.address();
}
unsigned short NWMUDPIO::GetPort() const
{
	if(IsTerminated())
		return 0;
	auto endPoint = m_socket->remote_endpoint();
	return endPoint.port();
}
std::string NWMUDPIO::GetLocalIP() const
{
	if(IsTerminated())
		return nwm::invalid_address();
	return nwm::to_string(m_socket->local_endpoint());
}
boost::asio::ip::address NWMUDPIO::GetLocalAddress() const
{
	if(IsTerminated())
		return boost::asio::ip::address();
	auto endPoint = m_socket->local_endpoint();
	return endPoint.address();
}
unsigned short NWMUDPIO::GetLocalPort() const
{
	if(IsTerminated())
		return 0;
	auto endPoint = m_socket->local_endpoint();
	return endPoint.port();
}

void NWMUDPIO::AsyncWrite(const NWMEndpoint &ep,const std::vector<boost::asio::mutable_buffer> &buffers,const std::function<void(const boost::system::error_code&,std::size_t)> &f)
{NWMUDPIOBase::AsyncWrite(m_socket.get(),buffers,ep,f);}
void NWMUDPIO::AsyncRead(const std::vector<boost::asio::mutable_buffer> &buffers,const std::function<void(const boost::system::error_code&,std::size_t)> &f,bool bPeek)
{NWMUDPIOBase::AsyncRead(m_socket.get(),buffers,m_remoteEndpoint,f,bPeek);}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
