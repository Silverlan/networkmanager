/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/nwm_connection.h"
#include "networkmanager/nwm_error_handle.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
NWMConnection::NWMConnection()
	: m_bShutDown(false),m_bClosing(false)
{
	try
	{
		ioService = std::make_unique<boost::asio::io_service>();
	}
	catch(std::exception e)
	{
		std::string err = "Unable to create IO Service: ";
		err += e.what();
		throw NWMException(err);
	}
}
NWMConnection::~NWMConnection()
{
	ioService = nullptr;
}
void NWMConnection::Run()
{
	if(!IsActive())
		return;
	Poll();
}
void NWMConnection::Poll()
{
	ioService->poll();
}
bool NWMConnection::IsActive() const {return (ioService != nullptr) ? true : false;}
bool NWMConnection::IsUDP() const {return false;}
bool NWMConnection::IsTCP() const {return false;}
void NWMConnection::Terminate()
{
	if(ioService == nullptr)
		return;
	CloseSocket();
	ioService->stop();
	m_bClosing = false;
}
void NWMConnection::SendPacket(const NetPacket&,const NWMEndpoint&,bool) {}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
