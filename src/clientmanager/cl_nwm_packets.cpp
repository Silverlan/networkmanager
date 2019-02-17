/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "clientmanager/legacy/cl_nwm_manager.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
bool NWMClient::HandlePacket(NWMIO *io,unsigned int id,NetPacket &packet)
{
	OnPacketReceived(io,id,packet);
	if(id == NWM_MESSAGE_IN_REGISTER_CONFIRMATION)
	{
		OnConnected();
		return true;
	}
	else if(id == NWM_MESSAGE_IN_DROPPED)
	{
		char reason = packet->Read<char>();
		OnDisconnected(CLIENT_DROPPED(reason));
		Close();
		return true;
	}
	else if(id == NWM_MESSAGE_IN_PONG)
	{
		Pong();
		return true;
	}
	return false;
}

void NWMClient::OnPacketReceived(NWMIO *io,unsigned int id,NetPacket &packet) {}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
