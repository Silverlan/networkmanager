/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkmanager/nwm_error_handle.h"
#include "networkmanager/nwm_boost.h"

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
std::string nwm::get_error_name(int id)
{
	switch(id) {
	case boost::asio::error::access_denied:
		return "Access denied";
	case boost::asio::error::address_family_not_supported:
		return "Address family not supported";
	case boost::asio::error::address_in_use:
		return "Address in use";
	case boost::asio::error::already_connected:
		return "Already connected";
	case boost::asio::error::already_started:
		return "Already started";
	case boost::asio::error::broken_pipe:
		return "Broken pipe";
	case boost::asio::error::connection_aborted:
		return "Connection aborted";
	case boost::asio::error::connection_refused:
		return "Connection refused";
	case boost::asio::error::connection_reset:
		return "Connection reset";
	case boost::asio::error::bad_descriptor:
		return "Bad descriptor";
	case boost::asio::error::fault:
		return "Fault";
	case boost::asio::error::host_unreachable:
		return "Host unreachable";
	case boost::asio::error::in_progress:
		return "In progress";
	case boost::asio::error::interrupted:
		return "Interrupted";
	case boost::asio::error::invalid_argument:
		return "Invalid argument";
	case boost::asio::error::message_size:
		return "Message size";
	case boost::asio::error::name_too_long:
		return "Name too long";
	case boost::asio::error::network_down:
		return "Network down";
	case boost::asio::error::network_reset:
		return "Network reset";
	case boost::asio::error::network_unreachable:
		return "Network unreachable";
	case boost::asio::error::no_descriptors:
		return "No descriptors";
	case boost::asio::error::no_buffer_space:
		return "No buffer space";
	case boost::asio::error::no_memory:
		return "No memory";
	case boost::asio::error::no_permission:
		return "No permission";
	case boost::asio::error::no_protocol_option:
		return "No protocol option";
	case boost::asio::error::not_connected:
		return "Not connected";
	case boost::asio::error::not_socket:
		return "Not socket";
	case boost::asio::error::operation_aborted:
		return "Operation aborted";
	case boost::asio::error::operation_not_supported:
		return "Operation not supported";
	case boost::asio::error::shut_down:
		return "Shut down";
	case boost::asio::error::timed_out:
		return "Timed out";
#ifdef __linux__
	case boost::asio::error::try_again:
		return "Would block / Try again";
#else
	case boost::asio::error::try_again:
		return "Try again";
	case boost::asio::error::would_block:
		return "Would block";
	case boost::asio::error::eof:
		return "End of file";
#endif
	};
	return "Unknown error";
}

///////////////////////////////

NWMException::NWMException(const std::string &err) : std::runtime_error(err) {}

///////////////////////////////

NWMErrorHandle::NWMErrorHandle() : m_errorHandle() {}

bool NWMErrorHandle::HandleError(const nwm::ErrorCode &error)
{
	if(!*error
#ifdef _WIN32
	  || error->value() == ERROR_MORE_DATA // More data available (We didn't read enough? Probably an invalid packet)
#endif
	)
		return true;
	if(m_errorHandle != nullptr)
		m_errorHandle(error);
	return false;
}

void NWMErrorHandle::SetErrorHandle(const std::function<void(const nwm::ErrorCode &)> &cbError) { m_errorHandle = cbError; }

std::string NWMErrorHandle::GetErrorName(int id) { return nwm::get_error_name(id); }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
