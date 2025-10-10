// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <vector>

export module pragma.server_manager:legacy.recipient_filter;

export import :legacy.session_handle;

export {
	enum class RPFilterType { INCLUDE, EXCLUDE };

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
		bool HasRecipient(const NWMServerClient *client);
		std::vector<SessionHandle> &get();
		size_t GetRecipientCount() const;
	};
}
