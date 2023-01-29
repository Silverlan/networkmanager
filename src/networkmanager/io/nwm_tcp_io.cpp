/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/nwm_boost.h"
#include "networkmanager/io/nwm_tcp_io.h"
#include "networkmanager/interface/nwm_manager.hpp"
#include "networkmanager/wrappers/nwm_impl_boost.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif

NWMTCPIO::NWMTCPIO(nwm::IOService &ioService) : NWMIO(), m_bNagleEnabled(false), m_bSocketInitialized(false)
{
	socket = std::make_unique<nwm::TCPSocket>(ioService); //,tcp::v6()); // Not needed?
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
		cast_socket(*socket)->set_option(tcp::no_delay(!m_bNagleEnabled));
}

void NWMTCPIO::Terminate()
{
	if(IsTerminated())
		return;
	if(cast_socket(*socket)->is_open()) {
		boost::system::error_code ec;
		cast_socket(*socket)->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		cast_socket(*socket)->close();
	}
	NWMIO::Terminate();
}

bool NWMTCPIO::IsTerminated() const { return NWMIO::IsTerminated(); }

std::string NWMTCPIO::GetIP() const
{
	if(IsTerminated())
		return nwm::invalid_address();
	return nwm::to_string(cast_socket(*socket)->remote_endpoint());
}

nwm::IPAddress NWMTCPIO::GetAddress() const
{
	if(IsTerminated())
		return nwm::IPAddress {};
	auto endPoint = cast_socket(*socket)->remote_endpoint();
	return endPoint.address();
}

unsigned short NWMTCPIO::GetPort() const
{
	if(IsTerminated())
		return 0;
	auto endPoint = cast_socket(*socket)->remote_endpoint();
	return endPoint.port();
}

std::string NWMTCPIO::GetLocalIP() const
{
	if(IsTerminated())
		return nwm::invalid_address();
	return nwm::to_string(cast_socket(*socket)->local_endpoint());
}
unsigned short NWMTCPIO::GetLocalPort() const
{
	if(IsTerminated())
		return 0;
	auto endPoint = cast_socket(*socket)->local_endpoint();
	return endPoint.port();
}
nwm::IPAddress NWMTCPIO::GetLocalAddress() const
{
	if(IsTerminated())
		return boost::asio::ip::address();
	auto endPoint = cast_socket(*socket)->local_endpoint();
	return endPoint.address();
}

void NWMTCPIO::AsyncWrite(const NWMEndpoint &, const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f)
{
	if(IsTerminated())
		return;
	std::vector<boost::asio::mutable_buffer> boostBuffers {};
	boostBuffers.reserve(buffers.size());
	for(auto &buf : buffers)
		boostBuffers.push_back(buf.GetBoostObject());
	boost::asio::async_write(*cast_socket(*socket), boostBuffers, f);
}

void NWMTCPIO::AsyncRead(const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f, bool bPeek)
{
	assert(bPeek == false);
	if(bPeek == true)
		throw std::logic_error("Peeking not supported for TCP protocol!");
	if(IsTerminated())
		return;
	std::vector<boost::asio::mutable_buffer> boostBuffers {};
	boostBuffers.reserve(buffers.size());
	for(auto &buf : buffers)
		boostBuffers.push_back(buf.GetBoostObject());
	boost::asio::async_read(*cast_socket(*socket), boostBuffers, f);
}

nwm::Protocol NWMTCPIO::GetProtocol() const { return nwm::Protocol::TCP; }

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
