/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SV_NWM_RECIPIENTFILTER_H__
#define __SV_NWM_RECIPIENTFILTER_H__

#include "servermanager/legacy/sv_nwm_sessionhandle.h"
#include <vector>

enum class RPFilterType { INCLUDE, EXCLUDE };

class NWMServerClient;
class RecipientFilter {
  private:
	std::vector<SessionHandle> m_sessions;
	RPFilterType m_type;
  public:
	RecipientFilter(RPFilterType type = RPFilterType::INCLUDE);
	void SetFilterType(RPFilterType type);
	RPFilterType GetFilterType() const;
	void Add(const SessionHandle &hSession);
	void Remove(const SessionHandle &hSession);
	void Add(NWMServerClient *client);
	void Remove(NWMServerClient *client);
	bool HasRecipient(const SessionHandle &hSession);
	bool HasRecipient(NWMServerClient *client);
	std::vector<SessionHandle> &get();
	size_t GetRecipientCount() const;
};

#endif
