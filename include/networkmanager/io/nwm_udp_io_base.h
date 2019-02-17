/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_UDP_IO_BASE_H__
#define __NWM_UDP_IO_BASE_H__

#include "nwm_io.h"
#include "nwm_io_header.h"

class NWMUDPIOBase
{
protected:
	virtual void AsyncWrite(udp::socket *socket,const std::vector<boost::asio::mutable_buffer> &buffers,const NWMEndpoint &endPoint,const std::function<void(const boost::system::error_code&,std::size_t)> &f);
	virtual void AsyncRead(udp::socket *socket,const std::vector<boost::asio::mutable_buffer> &buffers,const NWMEndpoint &endPoint,const std::function<void(const boost::system::error_code&,std::size_t)> &f,bool bPeek=false);
public:
	NWMUDPIOBase();
	virtual ~NWMUDPIOBase();
	virtual bool IsClosing() const=0;
	virtual std::string GetIP() const=0;
	virtual unsigned short GetPort() const=0;
	virtual void Terminate();
	virtual bool IsTerminated() const;
};

#endif