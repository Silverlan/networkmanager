// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_ENDPOINT_H__
#define __NWM_ENDPOINT_H__

#include <memory>
#include <string>
#include "wrappers/nwm_ip_address.hpp"
#include "wrappers/nwm_tcp.hpp"
#include "wrappers/nwm_tcp_endpoint.hpp"
#include "wrappers/nwm_udp.hpp"
#include "wrappers/nwm_udp_endpoint.hpp"

namespace boost {
	namespace asio {
		namespace ip {
			class address;
		};
	};
};
namespace nwm {
	std::string to_string(const std::string &ip, unsigned short port, bool bV6 = false);
	std::string to_string(const nwm::IPAddress &address, unsigned short port);
	std::string to_string(const boost::asio::ip::address &address, unsigned short port);
	std::string to_string(const nwm::UDPEndpoint &ep);
	std::string to_string(const nwm::TCPEndpoint &ep);
	std::string invalid_address();
};

class NWMBaseEndpoint;
class NWMTCPEndpoint;
class NWMUDPEndpoint;
class NWMEndpoint : public std::shared_ptr<NWMBaseEndpoint> {
  protected:
	NWMEndpoint(NWMBaseEndpoint *ep);
  public:
	NWMEndpoint();
	NWMEndpoint(NWMTCPEndpoint &ep);
	NWMEndpoint(NWMUDPEndpoint &ep);
	NWMEndpoint Copy() const;
	static NWMEndpoint CreateTCP(const nwm::TCPEndpoint &ep);
	static NWMEndpoint CreateUDP(const nwm::UDPEndpoint &ep);
	static NWMEndpoint CreateTCP(const nwm::TCP &ip, unsigned short port);
	static NWMEndpoint CreateUDP(const nwm::UDP &ip, unsigned short port);
	NWMBaseEndpoint *operator->() const;
	bool operator==(const NWMEndpoint &other) const;
	bool operator!=(const NWMEndpoint &other) const;
	bool operator==(const nwm::IPAddress &address) const;
	bool operator!=(const nwm::IPAddress &address) const;
	nwm::IPAddress GetAddress() const;
	unsigned short GetPort() const;
	std::string GetIP() const;
};

class NWMBaseEndpoint {
  protected:
	NWMBaseEndpoint();
  public:
	virtual bool IsTCP();
	virtual bool IsUDP();
};

class NWMUDPEndpoint : public NWMBaseEndpoint {
  protected:
	nwm::UDPEndpoint m_endPoint;
  public:
	NWMUDPEndpoint(const nwm::UDP &ip, unsigned short port);
	NWMUDPEndpoint(const nwm::UDPEndpoint &ep);
	NWMUDPEndpoint(NWMUDPEndpoint &ep);
	NWMUDPEndpoint(std::nullptr_t np);
	nwm::UDPEndpoint *operator->();
	nwm::UDPEndpoint *get();
	virtual bool IsUDP() override;
};

class NWMTCPEndpoint : public NWMBaseEndpoint {
  protected:
	nwm::TCPEndpoint m_endPoint;
  public:
	NWMTCPEndpoint(const nwm::TCP &ip, unsigned short port);
	NWMTCPEndpoint(const nwm::TCPEndpoint &ep);
	NWMTCPEndpoint(NWMTCPEndpoint &ep);
	NWMTCPEndpoint(std::nullptr_t np);
	nwm::TCPEndpoint *operator->();
	nwm::TCPEndpoint *get();
	virtual bool IsTCP() override;
};

#endif
