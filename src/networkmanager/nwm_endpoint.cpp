/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/nwm_endpoint.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
template<class T,class S>
	static std::string endPointToString(const T &ep,const S &protocolV6)
{
	return nwm::to_string(ep.address().to_string(),ep.port(),(ep.protocol() == protocolV6) ? true : false);
}

std::string nwm::to_string(const std::string &ip,unsigned short port,bool bV6)
{
	std::string address = ip;
	if(bV6 == true)
		address = std::string("[") +address +std::string("]");
	address += std::string(":") +std::to_string(port);
	return address;
}
std::string nwm::to_string(const boost::asio::ip::address &address,unsigned short port) {return nwm::to_string(address.to_string(),port,address.is_v6());}
std::string nwm::to_string(const boost::asio::ip::udp::endpoint &ep) {return endPointToString(ep,boost::asio::ip::udp::v6());}
std::string nwm::to_string(const boost::asio::ip::tcp::endpoint &ep) {return endPointToString(ep,boost::asio::ip::tcp::v6());}
std::string nwm::invalid_address() {return "[::]:0";}

////////////////////////

NWMEndpoint::NWMEndpoint(NWMBaseEndpoint *ep)
	: std::shared_ptr<NWMBaseEndpoint>(ep)
{}

NWMEndpoint::NWMEndpoint()
	: NWMEndpoint(nullptr)
{}

NWMEndpoint::NWMEndpoint(NWMTCPEndpoint &ep)
	: std::shared_ptr<NWMBaseEndpoint>(new NWMTCPEndpoint(*ep.get()))
{}
NWMEndpoint::NWMEndpoint(NWMUDPEndpoint &ep)
	: std::shared_ptr<NWMBaseEndpoint>(new NWMUDPEndpoint(*ep.get()))
{}

NWMEndpoint NWMEndpoint::Copy() const
{
	auto *ep = get();
	if(ep == nullptr)
		return NWMEndpoint();
	if(ep->IsTCP())
		return NWMEndpoint(*static_cast<NWMTCPEndpoint*>(ep));
	return NWMEndpoint(*static_cast<NWMUDPEndpoint*>(ep));
}

NWMEndpoint NWMEndpoint::CreateTCP(tcp::endpoint &ep)
{
	return NWMEndpoint(new NWMTCPEndpoint(ep));
}
NWMEndpoint NWMEndpoint::CreateUDP(udp::endpoint &ep)
{
	return NWMEndpoint(new NWMUDPEndpoint(ep));
}

NWMEndpoint NWMEndpoint::CreateTCP(const boost::asio::ip::tcp &ip,unsigned short port)
{
	return NWMEndpoint(new NWMTCPEndpoint(ip,port));
}

NWMEndpoint NWMEndpoint::CreateUDP(const boost::asio::ip::udp &ip,unsigned short port)
{
	return NWMEndpoint(new NWMUDPEndpoint(ip,port));
}

boost::asio::ip::address NWMEndpoint::GetAddress() const
{
	NWMBaseEndpoint *ep = get();
	if(ep == nullptr)
		return boost::asio::ip::address();
	if(ep->IsTCP())
	{
		boost::asio::ip::tcp::endpoint *epTCP = static_cast<NWMTCPEndpoint*>(ep)->get();
		if(epTCP == nullptr)
			return boost::asio::ip::address();
		return epTCP->address();
	}
	boost::asio::ip::udp::endpoint *epUDP = static_cast<NWMUDPEndpoint*>(ep)->get();
	if(epUDP == nullptr)
		return boost::asio::ip::address();
	return epUDP->address();
}

unsigned short NWMEndpoint::GetPort() const
{
	NWMBaseEndpoint *ep = get();
	if(ep == nullptr)
		return 0;
	if(ep->IsTCP())
	{
		boost::asio::ip::tcp::endpoint *epTCP = static_cast<NWMTCPEndpoint*>(ep)->get();
		if(epTCP == nullptr)
			return 0;
		return epTCP->port();
	}
	boost::asio::ip::udp::endpoint *epUDP = static_cast<NWMUDPEndpoint*>(ep)->get();
	if(epUDP == nullptr)
		return 0;
	return epUDP->port();
}

std::string NWMEndpoint::GetIP() const {return nwm::to_string(GetAddress(),GetPort());}

NWMBaseEndpoint *NWMEndpoint::operator->() const {return get();}

bool NWMEndpoint::operator==(const NWMEndpoint &other) const
{
	NWMBaseEndpoint *epThis = get();
	NWMBaseEndpoint *epOther = other.get();
	if(epThis == nullptr)
	{
		if(epOther == nullptr)
			return true;
		return false;
	}
	if(epThis->IsTCP())
	{
		if(!epOther->IsTCP())
			return false;
		auto *tcpEpThis = static_cast<NWMTCPEndpoint*>(epThis)->get();
		auto *tcpEpOther = static_cast<NWMTCPEndpoint*>(epOther)->get();
		if(tcpEpThis == tcpEpOther)
			return true;
		return (tcpEpThis->address() == tcpEpOther->address() && tcpEpThis->port() == tcpEpOther->port()) ? true : false;
	}
	if(!epOther->IsUDP())
		return false;
	auto *udpEpThis = static_cast<NWMUDPEndpoint*>(epThis)->get();
	auto *udpEpOther = static_cast<NWMUDPEndpoint*>(epOther)->get();
	if(udpEpThis == udpEpOther)
		return true;
	return (udpEpThis->address() == udpEpOther->address() && udpEpThis->port() == udpEpOther->port()) ? true : false;
}
bool NWMEndpoint::operator!=(const NWMEndpoint &other) const
{
	return (*this == other) ? false : true;
}

bool NWMEndpoint::operator==(const boost::asio::ip::address &address) const
{
	NWMBaseEndpoint *epThis = get();
	if(epThis == nullptr)
		return false;
	if(epThis->IsTCP())
		return (static_cast<NWMTCPEndpoint*>(epThis)->get()->address() == address) ? true : false;
	return (static_cast<NWMUDPEndpoint*>(epThis)->get()->address() == address) ? true : false;
}
bool NWMEndpoint::operator!=(const boost::asio::ip::address &address) const
{
	return (*this == address) ? false : true;
}

////////////////////////

NWMBaseEndpoint::NWMBaseEndpoint()
{}

bool NWMBaseEndpoint::IsUDP() {return false;}
bool NWMBaseEndpoint::IsTCP() {return false;}

////////////////////////

NWMUDPEndpoint::NWMUDPEndpoint(const udp::endpoint &ep)
	: m_endPoint(ep)
{}

NWMUDPEndpoint::NWMUDPEndpoint(const boost::asio::ip::udp &ip,unsigned short port)
	: NWMUDPEndpoint(udp::endpoint(ip,port))
{}

NWMUDPEndpoint::NWMUDPEndpoint(NWMUDPEndpoint &ep)
	: NWMUDPEndpoint(*(ep.get()))
{}

NWMUDPEndpoint::NWMUDPEndpoint(std::nullptr_t)
	: m_endPoint()
{}

udp::endpoint *NWMUDPEndpoint::get() {return &m_endPoint;}
udp::endpoint *NWMUDPEndpoint::operator->() {return &m_endPoint;}
bool NWMUDPEndpoint::IsUDP() {return true;}

////////////////////////

NWMTCPEndpoint::NWMTCPEndpoint(const tcp::endpoint &ep)
	: m_endPoint(ep)
{}

NWMTCPEndpoint::NWMTCPEndpoint(const boost::asio::ip::tcp &ip,unsigned short port)
	: NWMTCPEndpoint(tcp::endpoint(ip,port))
{}

NWMTCPEndpoint::NWMTCPEndpoint(NWMTCPEndpoint &ep)
	: NWMTCPEndpoint(*(ep.get()))
{}

NWMTCPEndpoint::NWMTCPEndpoint(std::nullptr_t)
	: m_endPoint()
{}

tcp::endpoint *NWMTCPEndpoint::get() {return &m_endPoint;}
tcp::endpoint *NWMTCPEndpoint::operator->() {return &m_endPoint;}
bool NWMTCPEndpoint::IsTCP() {return true;}

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
