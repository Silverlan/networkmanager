/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "servermanager/legacy/sv_nwm_manager.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
bool NWMServer::HandlePacket(const NWMEndpoint &ep,NWMSession *session,NWMServerClient *cl,unsigned int id,NetPacket &packet)
{
	OnPacketReceived(ep,session,cl,id,packet);
	if(cl == nullptr || !cl->IsFullyConnected())
		return true;
	cl->ResetLastUpdate();
	if(id == NWM_MESSAGE_IN_CLIENT_DISCONNECT)
	{
		DropClient(cl,CLIENT_DROPPED::DISCONNECTED,true);
		return true;
	}
	else if(id == NWM_MESSAGE_IN_PING)
	{
		unsigned short latency = packet->Read<unsigned short>();
		cl->SetLatency(latency);
		NetPacket p(NWM_MESSAGE_OUT_PONG);
		session->SendPacket(p);
		return true;
	}
	return (session != nullptr) ? HandlePacket(session,cl,id,packet) : false;
}

void NWMServer::OnPacketReceived(const NWMEndpoint &ep,NWMSession *session,NWMServerClient *cl,unsigned int id,NetPacket &packet) {}

bool NWMServer::HandlePacket(NWMSession *session,NWMServerClient *cl,unsigned int id,NetPacket &packet)
{
	return false;
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif

