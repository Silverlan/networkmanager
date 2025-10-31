// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <boost/asio.hpp>

module pragma.network_manager;

import :endpoint;
import :boost_cast;

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
template<class T, class S>
static std::string endPointToString(const T &ep, const S &protocolV6)
{
	return nwm::to_string(ep.address().to_string(), ep.port(), (ep.protocol() == protocolV6) ? true : false);
}

std::string nwm::to_string(const std::string &ip, unsigned short port, bool bV6)
{
	std::string address = ip;
	if(bV6 == true)
		address = std::string("[") + address + std::string("]");
	address += std::string(":") + std::to_string(port);
	return address;
}
std::string nwm::to_string(const nwm::IPAddress &address, unsigned short port) { return nwm::to_string(address->to_string(), port, address->is_v6()); }
std::string nwm::to_string(const boost::asio::ip::address &address, unsigned short port) { return nwm::to_string(address.to_string(), port, address.is_v6()); }
std::string nwm::to_string(const boost::asio::ip::udp::endpoint &ep) { return endPointToString(ep, boost::asio::ip::udp::v6()); }
std::string nwm::to_string(const boost::asio::ip::tcp::endpoint &ep) { return endPointToString(ep, boost::asio::ip::tcp::v6()); }
std::string nwm::to_string(const nwm::UDPEndpoint &ep) { return to_string(*cast_endpoint(ep)); }
std::string nwm::to_string(const nwm::TCPEndpoint &ep) { return to_string(*cast_endpoint(ep)); }
std::string nwm::invalid_address() { return "[::]:0"; }

////////////////////////

NWMEndpoint::NWMEndpoint(NWMBaseEndpoint *ep) : std::shared_ptr<NWMBaseEndpoint>(ep) {}

NWMEndpoint::NWMEndpoint() : NWMEndpoint(nullptr) {}

NWMEndpoint::NWMEndpoint(NWMTCPEndpoint &ep) : std::shared_ptr<NWMBaseEndpoint>(new NWMTCPEndpoint(*ep.get())) {}
NWMEndpoint::NWMEndpoint(NWMUDPEndpoint &ep) : std::shared_ptr<NWMBaseEndpoint>(new NWMUDPEndpoint(*ep.get())) {}

NWMEndpoint NWMEndpoint::Copy() const
{
	auto *ep = get();
	if(ep == nullptr)
		return NWMEndpoint();
	if(ep->IsTCP())
		return NWMEndpoint(*static_cast<NWMTCPEndpoint *>(ep));
	return NWMEndpoint(*static_cast<NWMUDPEndpoint *>(ep));
}

NWMEndpoint NWMEndpoint::CreateTCP(const nwm::TCPEndpoint &ep) { return NWMEndpoint(new NWMTCPEndpoint(ep)); }
NWMEndpoint NWMEndpoint::CreateUDP(const nwm::UDPEndpoint &ep) { return NWMEndpoint(new NWMUDPEndpoint(ep)); }

NWMEndpoint NWMEndpoint::CreateTCP(const nwm::TCP &ip, unsigned short port) { return NWMEndpoint(new NWMTCPEndpoint(ip, port)); }

NWMEndpoint NWMEndpoint::CreateUDP(const nwm::UDP &ip, unsigned short port) { return NWMEndpoint(new NWMUDPEndpoint(ip, port)); }

nwm::IPAddress NWMEndpoint::GetAddress() const
{
	NWMBaseEndpoint *ep = get();
	if(ep == nullptr)
		return nwm::IPAddress {};
	if(ep->IsTCP()) {
		auto *epTCP = static_cast<NWMTCPEndpoint *>(ep)->get();
		if(epTCP == nullptr)
			return nwm::IPAddress {};
		return nwm::cast_endpoint(*epTCP)->address();
	}
	auto *epUDP = static_cast<NWMUDPEndpoint *>(ep)->get();
	if(epUDP == nullptr)
		return nwm::IPAddress {};
	return nwm::cast_endpoint(*epUDP)->address();
}

unsigned short NWMEndpoint::GetPort() const
{
	NWMBaseEndpoint *ep = get();
	if(ep == nullptr)
		return 0;
	if(ep->IsTCP()) {
		auto *epTCP = static_cast<NWMTCPEndpoint *>(ep)->get();
		if(epTCP == nullptr)
			return 0;
		return nwm::cast_endpoint(*epTCP)->port();
	}
	auto *epUDP = static_cast<NWMUDPEndpoint *>(ep)->get();
	if(epUDP == nullptr)
		return 0;
	return nwm::cast_endpoint(*epUDP)->port();
}

std::string NWMEndpoint::GetIP() const { return nwm::to_string(GetAddress(), GetPort()); }

NWMBaseEndpoint *NWMEndpoint::operator->() const { return get(); }

bool NWMEndpoint::operator==(const NWMEndpoint &other) const
{
	NWMBaseEndpoint *epThis = get();
	NWMBaseEndpoint *epOther = other.get();
	if(epThis == nullptr) {
		if(epOther == nullptr)
			return true;
		return false;
	}
	if(epThis->IsTCP()) {
		if(!epOther->IsTCP())
			return false;
		auto *tcpEpThis = static_cast<NWMTCPEndpoint *>(epThis)->get();
		auto *tcpEpOther = static_cast<NWMTCPEndpoint *>(epOther)->get();
		if(tcpEpThis == tcpEpOther)
			return true;
		return (nwm::cast_endpoint(*tcpEpThis)->address() == nwm::cast_endpoint(*tcpEpOther)->address() && nwm::cast_endpoint(*tcpEpThis)->port() == nwm::cast_endpoint(*tcpEpOther)->port()) ? true : false;
	}
	if(!epOther->IsUDP())
		return false;
	auto *udpEpThis = static_cast<NWMUDPEndpoint *>(epThis)->get();
	auto *udpEpOther = static_cast<NWMUDPEndpoint *>(epOther)->get();
	if(udpEpThis == udpEpOther)
		return true;
	return (nwm::cast_endpoint(*udpEpThis)->address() == nwm::cast_endpoint(*udpEpOther)->address() && nwm::cast_endpoint(*udpEpThis)->port() == nwm::cast_endpoint(*udpEpOther)->port()) ? true : false;
}
bool NWMEndpoint::operator!=(const NWMEndpoint &other) const { return (*this == other) ? false : true; }

bool NWMEndpoint::operator==(const nwm::IPAddress &address) const
{
	NWMBaseEndpoint *epThis = get();
	if(epThis == nullptr)
		return false;
	if(epThis->IsTCP())
		return (nwm::cast_endpoint(*static_cast<NWMTCPEndpoint *>(epThis)->get())->address() == *address) ? true : false;
	return (nwm::cast_endpoint(*static_cast<NWMUDPEndpoint *>(epThis)->get())->address() == *address) ? true : false;
}
bool NWMEndpoint::operator!=(const nwm::IPAddress &address) const { return (*this == address) ? false : true; }

////////////////////////

NWMBaseEndpoint::NWMBaseEndpoint() {}

bool NWMBaseEndpoint::IsUDP() { return false; }
bool NWMBaseEndpoint::IsTCP() { return false; }

////////////////////////

static boost::asio::ip::udp::endpoint tmpUDPEndpoint {};
static boost::asio::ip::udp::endpoint *create_boost_udp_endpoint(const nwm::UDP &ip, unsigned short port)
{
	tmpUDPEndpoint = boost::asio::ip::udp::endpoint {*ip, port};
	return &tmpUDPEndpoint;
}

NWMUDPEndpoint::NWMUDPEndpoint(const nwm::UDPEndpoint &ep) : m_endPoint(ep) {}

NWMUDPEndpoint::NWMUDPEndpoint(const nwm::UDP &ip, unsigned short port) : NWMUDPEndpoint(create_boost_udp_endpoint(*ip, port)) {}

NWMUDPEndpoint::NWMUDPEndpoint(NWMUDPEndpoint &ep) : NWMUDPEndpoint(*(ep.get())) {}

NWMUDPEndpoint::NWMUDPEndpoint(std::nullptr_t) : m_endPoint() {}

nwm::UDPEndpoint *NWMUDPEndpoint::get() { return &m_endPoint; }
nwm::UDPEndpoint *NWMUDPEndpoint::operator->() { return &m_endPoint; }
bool NWMUDPEndpoint::IsUDP() { return true; }

////////////////////////

static boost::asio::ip::tcp::endpoint tmpTCPEndpoint {};
static boost::asio::ip::tcp::endpoint *create_boost_tcp_endpoint(const nwm::TCP &ip, unsigned short port)
{
	tmpTCPEndpoint = boost::asio::ip::tcp::endpoint {*ip, port};
	return &tmpTCPEndpoint;
}

NWMTCPEndpoint::NWMTCPEndpoint(const nwm::TCPEndpoint &ep) : m_endPoint(ep) {}

NWMTCPEndpoint::NWMTCPEndpoint(const nwm::TCP &ip, unsigned short port) : NWMTCPEndpoint(create_boost_tcp_endpoint(*ip, port)) {}

NWMTCPEndpoint::NWMTCPEndpoint(NWMTCPEndpoint &ep) : NWMTCPEndpoint(*(ep.get())) {}

NWMTCPEndpoint::NWMTCPEndpoint(std::nullptr_t) : m_endPoint() {}

nwm::TCPEndpoint *NWMTCPEndpoint::get() { return &m_endPoint; }
nwm::TCPEndpoint *NWMTCPEndpoint::operator->() { return &m_endPoint; }
bool NWMTCPEndpoint::IsTCP() { return true; }

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
