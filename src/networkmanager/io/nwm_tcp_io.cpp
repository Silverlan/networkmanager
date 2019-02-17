/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/io/nwm_tcp_io.h"
#include "networkmanager/interface/nwm_manager.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NWMTCPIO::NWMTCPIO(boost::asio::io_service &ioService)
	: NWMIO(),m_bNagleEnabled(false),m_bSocketInitialized(false)
{
	socket = std::make_unique<tcp::socket>(ioService);//,tcp::v6()); // Not needed?
}

NWMTCPIO::~NWMTCPIO()
{
	Terminate();
	socket = nullptr;
}

void NWMTCPIO::InitializeSocket()
{
	m_bSocketInitialized = true;
	SetNagleAlgorithmEnabled(m_bNagleEnabled);
}

void NWMTCPIO::SetNagleAlgorithmEnabled(bool b)
{
	m_bNagleEnabled = b;
	if(m_bSocketInitialized == false)
		return;
	if(socket != nullptr)
		socket->set_option(tcp::no_delay(!m_bNagleEnabled));
}

void NWMTCPIO::Terminate()
{
	if(IsTerminated())
		return;
	if(socket->is_open())
	{
		boost::system::error_code ec;
		socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
		socket->close();
	}
	NWMIO::Terminate();
}

bool NWMTCPIO::IsTerminated() const
{
	return NWMIO::IsTerminated();
}

std::string NWMTCPIO::GetIP() const
{
	if(IsTerminated())
		return nwm::invalid_address();
	return nwm::to_string(socket->remote_endpoint());
}

boost::asio::ip::address NWMTCPIO::GetAddress() const
{
	if(IsTerminated())
		return boost::asio::ip::address();
	auto endPoint = socket->remote_endpoint();
	return endPoint.address();
}

unsigned short NWMTCPIO::GetPort() const
{
	if(IsTerminated())
		return 0;
	auto endPoint = socket->remote_endpoint();
	return endPoint.port();
}

std::string NWMTCPIO::GetLocalIP() const
{
	if(IsTerminated())
		return nwm::invalid_address();
	return nwm::to_string(socket->local_endpoint());
}
unsigned short NWMTCPIO::GetLocalPort() const
{
	if(IsTerminated())
		return 0;
	auto endPoint = socket->local_endpoint();
	return endPoint.port();
}
boost::asio::ip::address NWMTCPIO::GetLocalAddress() const
{
	if(IsTerminated())
		return boost::asio::ip::address();
	auto endPoint = socket->local_endpoint();
	return endPoint.address();
}

void NWMTCPIO::AsyncWrite(const NWMEndpoint&,const std::vector<boost::asio::mutable_buffer> &buffers,const std::function<void(const boost::system::error_code&,std::size_t)> &f)
{
	if(IsTerminated())
		return;
	boost::asio::async_write(*socket,
		buffers,
		f
	);
}

void NWMTCPIO::AsyncRead(const std::vector<boost::asio::mutable_buffer> &buffers,const std::function<void(const boost::system::error_code&,std::size_t)> &f,bool bPeek)
{
	assert(bPeek == false);
	if(bPeek == true)
		throw std::logic_error("Peeking not supported for TCP protocol!");
	if(IsTerminated())
		return;
	boost::asio::async_read(
		*socket,
		buffers,
		f
	);
}

nwm::Protocol NWMTCPIO::GetProtocol() const {return nwm::Protocol::TCP;}

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
