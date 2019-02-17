/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/io/nwm_udp_io_base.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NWMUDPIOBase::NWMUDPIOBase()
{}

NWMUDPIOBase::~NWMUDPIOBase()
{}

void NWMUDPIOBase::Terminate()
{}

bool NWMUDPIOBase::IsTerminated() const
{
	if(socket == nullptr)
		return true;
	return false;
}
void NWMUDPIOBase::AsyncWrite(udp::socket *socket,const std::vector<boost::asio::mutable_buffer> &buffers,const NWMEndpoint &endPoint,const std::function<void(const boost::system::error_code&,std::size_t)> &f)
{
	if(IsTerminated())
		return;
	NWMUDPEndpoint *ep = static_cast<NWMUDPEndpoint*>(endPoint.get());
	if(ep == nullptr)
		return;
	boost::asio::ip::udp::endpoint *epUDP = ep->get();
	if(epUDP == nullptr)
		return;
	socket->async_send_to(
		buffers,
		*epUDP,
		f
	);
}

void NWMUDPIOBase::AsyncRead(udp::socket *socket,const std::vector<boost::asio::mutable_buffer> &buffers,const NWMEndpoint &endPoint,const std::function<void(const boost::system::error_code&,std::size_t)> &f,bool bPeek)
{
	if(IsTerminated())
		return;
	NWMUDPEndpoint *ep = static_cast<NWMUDPEndpoint*>(endPoint.get());
	if(ep == nullptr)
		return;
	boost::asio::ip::udp::endpoint *epUDP = ep->get();
	if(epUDP == nullptr)
		return;
	int32_t flags = 0;
	if(bPeek == true)
		flags |= boost::asio::socket_base::message_peek;
	socket->async_receive_from(
		buffers,
		*epUDP,flags,
		f
	);
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
