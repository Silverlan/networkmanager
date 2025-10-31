// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

module pragma.network_manager;

import :io.udp;
import :boost_cast;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif

using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;

NWMUDPIO::NWMUDPIO(nwm::IOService &ioService, unsigned short localPort) : NWMIO(), NWMUDPIOBase()
{
	try {
		udp::resolver resolver(*ioService);
		boost::system::error_code ec;
#if NWM_USE_IPV6 == 0
		auto results = resolver.resolve(udp::v4(), "0.0.0.0", std::to_string(localPort), ec);
#else
		auto results = resolver.resolve("::1", std::to_string(localPort), ec);
#endif

		if(ec)
			throw boost::system::system_error(ec);

		udp::endpoint localEndpoint = *results.begin();
		m_localEndpoint = NWMEndpoint::CreateUDP(nwm::UDPEndpoint {&localEndpoint});
		m_socket = std::make_unique<nwm::UDPSocket>(ioService);
#if NWM_USE_IPV6 == 0
		cast_socket(*m_socket)->open(udp::v4());
#else
		cast_socket(*m_socket)->open(udp::v6());
#endif
		cast_socket(*m_socket)->bind(localEndpoint);
	}
	catch(std::exception &e) {
		m_socket = nullptr;
		throw NWMException(e.what());
	}
}

NWMUDPIO::~NWMUDPIO()
{
	CloseSocket();
	Terminate();
}

nwm::Protocol NWMUDPIO::GetProtocol() const { return nwm::Protocol::UDP; }
bool NWMUDPIO::IsTerminated() const { return NWMIO::IsTerminated() || NWMUDPIOBase::IsTerminated() || m_socket == nullptr || !cast_socket(*m_socket)->is_open(); }

NWMEndpoint &NWMUDPIO::GetLocalEndPoint() { return m_localEndpoint; }

void NWMUDPIO::CloseSocket()
{
	if(m_socket == nullptr)
		return;
	if(cast_socket(*m_socket)->is_open()) {
		//m_socket->shutdown(boost::asio::socket_base::shutdown_type::shutdown_both); // Not required for UDP sockets
		cast_socket(*m_socket)->close();
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
	return nwm::to_string(cast_socket(*m_socket)->remote_endpoint());
}

nwm::IPAddress NWMUDPIO::GetAddress() const
{
	if(IsTerminated())
		return boost::asio::ip::address();
	auto endPoint = cast_socket(*m_socket)->remote_endpoint();
	return endPoint.address();
}
unsigned short NWMUDPIO::GetPort() const
{
	if(IsTerminated())
		return 0;
	auto endPoint = cast_socket(*m_socket)->remote_endpoint();
	return endPoint.port();
}
std::string NWMUDPIO::GetLocalIP() const
{
	if(IsTerminated())
		return nwm::invalid_address();
	return nwm::to_string(cast_socket(*m_socket)->local_endpoint());
}
nwm::IPAddress NWMUDPIO::GetLocalAddress() const
{
	if(IsTerminated())
		return boost::asio::ip::address();
	auto endPoint = cast_socket(*m_socket)->local_endpoint();
	return endPoint.address();
}
unsigned short NWMUDPIO::GetLocalPort() const
{
	if(IsTerminated())
		return 0;
	auto endPoint = cast_socket(*m_socket)->local_endpoint();
	return endPoint.port();
}

void NWMUDPIO::AsyncWrite(const NWMEndpoint &ep, const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f) { NWMUDPIOBase::AsyncWrite(m_socket.get(), buffers, ep, f); }
void NWMUDPIO::AsyncRead(const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f, bool bPeek) { NWMUDPIOBase::AsyncRead(m_socket.get(), buffers, m_remoteEndpoint, f, bPeek); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
