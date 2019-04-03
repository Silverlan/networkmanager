/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_CLIENTHANDLE_HPP__
#define __SV_NWM_CLIENTHANDLE_HPP__

#include <memory>
#include <string>
#include <sharedutils/def_handle.h>

namespace nwm
{
	class ServerClient;
	DECLARE_BASE_HANDLE(,ServerClient,ServerClientBase);

	class Server;
	enum class ClientDropped : int8_t;
	class ServerClientHandle
		: public nwm::ServerClientBaseHandle
	{
	public:
		ServerClientHandle();
		ServerClientHandle(ServerClient *cl);
		ServerClientHandle(const ServerClientHandle &hSession);
		virtual ~ServerClientHandle() override;
		bool operator==(const ServerClientHandle &hOther) const;
		bool operator!=(const ServerClientHandle &hOther) const;
		Server *GetManager() const;
		void Drop(ClientDropped e);
		void Invalidate();
		bool IsValid() const;
	protected:
		void Initialize();
		friend ServerClient;
	private:
		mutable Server *m_manager = nullptr;
	};
};

#endif
