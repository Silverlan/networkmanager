// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/nwm_boost.h"
#include "networkmanager/io/nwm_udp_io_base.h"
#include "networkmanager/wrappers/nwm_impl_boost.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMUDPIOBase::NWMUDPIOBase() {}

NWMUDPIOBase::~NWMUDPIOBase() {}

void NWMUDPIOBase::Terminate() {}

bool NWMUDPIOBase::IsTerminated() const
{
	if(!socket)
		return true;
	return false;
}
void NWMUDPIOBase::AsyncWrite(nwm::UDPSocket *socket, const std::vector<nwm::MutableBuffer> &buffers, const NWMEndpoint &endPoint, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f)
{
	if(IsTerminated())
		return;
	NWMUDPEndpoint *ep = static_cast<NWMUDPEndpoint *>(endPoint.get());
	if(ep == nullptr)
		return;
	auto *epUDP = ep->get();
	if(epUDP == nullptr)
		return;
	std::vector<boost::asio::mutable_buffer> boostBuffers {};
	boostBuffers.reserve(buffers.size());
	for(auto &buf : buffers)
		boostBuffers.push_back(*buf);
	cast_socket(*socket)->async_send_to(boostBuffers, *cast_endpoint(*epUDP), f);
}

void NWMUDPIOBase::AsyncRead(nwm::UDPSocket *socket, const std::vector<nwm::MutableBuffer> &buffers, const NWMEndpoint &endPoint, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f, bool bPeek)
{
	if(IsTerminated())
		return;
	NWMUDPEndpoint *ep = static_cast<NWMUDPEndpoint *>(endPoint.get());
	if(ep == nullptr)
		return;
	auto *epUDP = ep->get();
	if(epUDP == nullptr)
		return;
	int32_t flags = 0;
	if(bPeek == true)
		flags |= boost::asio::socket_base::message_peek;
	std::vector<boost::asio::mutable_buffer> boostBuffers {};
	boostBuffers.reserve(buffers.size());
	for(auto &buf : buffers)
		boostBuffers.push_back(*buf);
	cast_socket(*socket)->async_receive_from(boostBuffers, *cast_endpoint(*epUDP), flags, [f](const boost::system::error_code &errCode, std::size_t n) { f(errCode, n); });
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
