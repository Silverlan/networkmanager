/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_ENDPOINT_H__
#define __NWM_ENDPOINT_H__

#include "nwm_boost.h"
#include <memory>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

namespace nwm
{
	std::string to_string(const std::string &ip,unsigned short port,bool bV6=false);
	std::string to_string(const boost::asio::ip::address &address,unsigned short port);
	std::string to_string(const boost::asio::ip::udp::endpoint &ep);
	std::string to_string(const boost::asio::ip::tcp::endpoint &ep);
	std::string invalid_address();
};

class NWMBaseEndpoint;
class NWMTCPEndpoint;
class NWMUDPEndpoint;
class NWMEndpoint
	: public std::shared_ptr<NWMBaseEndpoint>
{
protected:
	NWMEndpoint(NWMBaseEndpoint *ep);
public:
	NWMEndpoint();
	NWMEndpoint(NWMTCPEndpoint &ep);
	NWMEndpoint(NWMUDPEndpoint &ep);
	NWMEndpoint Copy() const;
	static NWMEndpoint CreateTCP(tcp::endpoint &ep);
	static NWMEndpoint CreateUDP(udp::endpoint &ep);
	static NWMEndpoint CreateTCP(const boost::asio::ip::tcp &ip,unsigned short port);
	static NWMEndpoint CreateUDP(const boost::asio::ip::udp &ip,unsigned short port);
	NWMBaseEndpoint *operator->() const;
	bool operator==(const NWMEndpoint &other) const;
	bool operator!=(const NWMEndpoint &other) const;
	bool operator==(const boost::asio::ip::address &address) const;
	bool operator!=(const boost::asio::ip::address &address) const;
	boost::asio::ip::address GetAddress() const;
	unsigned short GetPort() const;
	std::string GetIP() const;
};

class NWMBaseEndpoint
{
protected:
	NWMBaseEndpoint();
public:
	virtual bool IsTCP();
	virtual bool IsUDP();
};

class NWMUDPEndpoint
	: public NWMBaseEndpoint
{
protected:
	udp::endpoint m_endPoint;
public:
	NWMUDPEndpoint(const boost::asio::ip::udp &ip,unsigned short port);
	NWMUDPEndpoint(const udp::endpoint &ep);
	NWMUDPEndpoint(NWMUDPEndpoint &ep);
        NWMUDPEndpoint(std::nullptr_t np);
	udp::endpoint *operator->();
	udp::endpoint *get();
	virtual bool IsUDP() override;
};

class NWMTCPEndpoint
	: public NWMBaseEndpoint
{
protected:
	tcp::endpoint m_endPoint;
public:
	NWMTCPEndpoint(const boost::asio::ip::tcp &ip,unsigned short port);
	NWMTCPEndpoint(const tcp::endpoint &ep);
	NWMTCPEndpoint(NWMTCPEndpoint &ep);
        NWMTCPEndpoint(std::nullptr_t np);
	tcp::endpoint *operator->();
	tcp::endpoint *get();
	virtual bool IsTCP() override;
};

#endif
