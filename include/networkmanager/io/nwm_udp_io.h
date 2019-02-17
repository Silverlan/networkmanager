/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_UDP_IO_H__
#define __NWM_UDP_IO_H__

#include "nwm_io.h"
#include "nwm_udp_io_base.h"

class NWMUDPIO
	: public NWMIO,public NWMUDPIOBase
{
protected:
	NWMUDPIO(boost::asio::io_service &ioService,unsigned short localPort);
	std::unique_ptr<udp::socket> m_socket;
	NWMEndpoint m_localEndpoint;
	virtual void Terminate() override;
	virtual void AsyncWrite(const NWMEndpoint &ep,const std::vector<boost::asio::mutable_buffer> &buffers,const std::function<void(const boost::system::error_code&,std::size_t)> &f) override;
	virtual void AsyncRead(const std::vector<boost::asio::mutable_buffer> &buffers,const std::function<void(const boost::system::error_code&,std::size_t)> &f,bool bPeek=false) override;
	virtual void CloseSocket();
public:
	virtual ~NWMUDPIO() override;
	virtual std::string GetIP() const override;
	virtual boost::asio::ip::address GetAddress() const override;
	virtual unsigned short GetPort() const override;
	virtual std::string GetLocalIP() const override;
	virtual boost::asio::ip::address GetLocalAddress() const override;
	virtual unsigned short GetLocalPort() const override;
	virtual bool IsTerminated() const override;
	virtual nwm::Protocol GetProtocol() const override;
	NWMEndpoint &GetLocalEndPoint();
};

#endif