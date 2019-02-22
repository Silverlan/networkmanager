/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_MANAGER_CREATE_HPP__
#define __SV_NWM_MANAGER_CREATE_HPP__

#include <servermanager/interface/sv_nwm_manager.hpp>
#include <servermanager/connection/sv_nwm_udpconnection.h>
#include <servermanager/connection/sv_nwm_tcpconnection.h>

template<class T>
	std::shared_ptr<T> nwm::Server::CreateClient()
{
	return std::shared_ptr<T>(new T(this));
}

template<class T>
	std::unique_ptr<T> nwm::Server::Create(uint16_t tcpPort,uint16_t udpPort,ConnectionType conType)
{
	std::shared_ptr<SVNWMUDPConnection> udp;
	if(conType == ConnectionType::UDP || conType == ConnectionType::TCPUDP)
	{
		udp = std::shared_ptr<SVNWMUDPConnection>(new SVNWMUDPConnection(udpPort));
		udp->Initialize();
	}
	std::shared_ptr<SVNWMTCPConnection> tcp;
	if(conType == ConnectionType::TCP || conType == ConnectionType::TCPUDP)
	{
		tcp = std::shared_ptr<SVNWMTCPConnection>(new SVNWMTCPConnection(tcpPort));
		tcp->Initialize();
	}
	return std::unique_ptr<T>(new T(udp,tcp));
}

template<class T>
	std::unique_ptr<T> nwm::Server::Create(uint16_t port,ConnectionType conType) {return Create<T>(port,port,conType);}

#endif
