// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "servermanager/interface/sv_nwm_clienthandle.hpp"
#include "servermanager/interface/sv_nwm_manager.hpp"
#include "servermanager/interface/sv_nwm_serverclient.hpp"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",off)
#endif
namespace nwm
{
	DEFINE_BASE_HANDLE(,ServerClient,ServerClientBase);
};

nwm::ServerClientHandle::ServerClientHandle()
	: ServerClientBaseHandle()
{
	Initialize();
}
nwm::ServerClientHandle::ServerClientHandle(ServerClient *cl)
	: ServerClientBaseHandle(cl)
{
	Initialize();
}

nwm::ServerClientHandle::ServerClientHandle(const ServerClientHandle &hSession)
	: ServerClientBaseHandle(hSession.get())
{
	m_manager = hSession.GetManager();
}

nwm::ServerClientHandle::~ServerClientHandle() {}

bool nwm::ServerClientHandle::IsValid() const {return ServerClientBaseHandle::IsValid();}

bool nwm::ServerClientHandle::operator==(const ServerClientHandle &hOther) const {return (get() == hOther.get()) ? true : false;}
bool nwm::ServerClientHandle::operator!=(const ServerClientHandle &hOther) const {return (*this == hOther) ? false : true;}

void nwm::ServerClientHandle::Initialize()
{
	auto *cl = get();
	if(cl == nullptr)
	{
		m_manager = nullptr;
		return;
	}
	m_manager = cl->GetManager();
}

nwm::Server *nwm::ServerClientHandle::GetManager() const {return m_manager;}

void nwm::ServerClientHandle::Invalidate() {ServerClientBaseHandle::Invalidate();}

void nwm::ServerClientHandle::Drop(ClientDropped e)
{
	if(m_manager == nullptr)
		return;
	m_manager->DropClient(get(),e);
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("",on)
#endif
