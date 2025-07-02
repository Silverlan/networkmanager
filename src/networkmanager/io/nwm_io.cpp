// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include "networkmanager/nwm_boost.h"
#include "networkmanager/io/nwm_io.h"
#include "networkmanager/nwm_defines.h"
#include "networkmanager/interface/nwm_manager.hpp"
#include "networkmanager/wrappers/nwm_boost_wrapper_impl.hpp"
#include <sharedutils/util.h>

#define NWM_SEGMENT_LARGE_PACKAGES

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
NWMIO::NWMIO() : NWMIOHeader(), NWMIOBase(), m_readPacket(nullptr), m_writeSizeLeft(0), m_readSizeLeft(0), m_packetHandle(), m_bWriting(false), m_bReading(false), m_bReady(false) {}

NWMIO::~NWMIO() { ClearPackets(); }

bool NWMIO::IsWriting() { return m_bWriting; }
bool NWMIO::IsReading() { return m_bReading; }

void NWMIO::ClearPackets()
{
	while(!m_packets.empty()) {
		PacketQueueItem &item = m_packets.front();
		item.packet = nullptr;
		m_packets.pop();
	}
	while(!m_initPackets.empty())
		m_initPackets.pop();
}

bool NWMIO::HandleError(const nwm::ErrorCode &error)
{
	bool r = NWMErrorHandle::HandleError(error);
	if(r == false) {
#ifdef NWM_VERBOSE
		std::cout << "[NWM] IO Error: " << GetErrorName(error.value()) << " (" << error.message() << ")" << std::endl;
#endif
		OnError(error);
		Close(true);
	}
	return r;
}

void NWMIO::SetReady()
{
	if(m_bReady == true)
		return;
	m_tLastMessage = util::Clock::now();
	m_bReady = true;
	while(!m_initPackets.empty()) {
		PacketQueueItem &item = m_initPackets.front();
		NetPacket &packet = item.packet;
		SendPacket(packet, true);
		m_initPackets.pop();
	}
}
bool NWMIO::IsReady() const { return m_bReady; }
bool NWMIO::IsTCP() const { return GetProtocol() == nwm::Protocol::TCP; }
bool NWMIO::IsUDP() const { return GetProtocol() == nwm::Protocol::UDP; }

void NWMIO::OnError(const nwm::ErrorCode &error)
{
	NWMIOBase::OnError(error);
	while(!m_packets.empty())
		m_packets.pop();
	while(!m_initPackets.empty())
		m_initPackets.pop();
}

void NWMIO::SendPacket(const PacketQueueItem &item)
{
	if(IsTerminated()) {
#if NWM_VERBOSE >= 2
		std::cout << "[NWM] Unable to send packet " << item.packet.GetMessageID() << ": Session is terminated!" << std::endl;
#endif
		return;
	}
#if NWM_VERBOSE >= 2
	std::cout << "[NWM] Adding packet " << item.packet.GetMessageID() << " to queue..." << std::endl;
#endif
	if(item.own == false) {
		if(IsClosing()) {
#if NWM_VERBOSE >= 2
			std::cout << "[NWM] Unable to send packet " << item.packet.GetMessageID() << ": Session is closing!" << std::endl;
#endif
			return;
		}
		if(m_bReady == false) {
#if NWM_VERBOSE >= 2
			std::cout << "[NWM] Queueing packet " << item.packet.GetMessageID() << " to be sent when session is initialized!" << std::endl;
#endif
			m_initPackets.push(item);
			return;
		}
	}
	else if(m_bReady == false)
		return;
#if NWM_VERBOSE >= 2
	std::cout << "[NWM] Queueing packet " << item.packet.GetMessageID() << "!" << std::endl;
#endif
	m_packets.push(item);
	if(m_packets.size() == 1 && m_bWriting == false)
		SendNextPacket();
}
void NWMIO::SendPacket(const NetPacket &packet, const NWMEndpoint &ep, bool bOwn) { SendPacket(PacketQueueItem(packet, ep, bOwn)); }
void NWMIO::SendPacket(const NetPacket &packet, bool bOwn) { SendPacket(packet, m_remoteEndpoint, bOwn); }
void NWMIO::SendPacket(const NetPacket &packet) { SendPacket(packet, false); }

void NWMIO::SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket) { m_packetHandle = cbPacket; }
bool NWMIO::ShouldTerminate() { return (!m_packets.empty() || m_bWriting == true || m_bReading == true) ? false : NWMIOBase::ShouldTerminate(); }

void NWMIO::Terminate()
{
	if(m_bTerminated == true)
		return;
	ClearPackets();
	m_bReady = false;
	NWMIOBase::Terminate();
}

static bool write_packet_header_data(DataStream &header, NetPacket &packet, uint32_t size)
{
	header->Write<uint16_t>(packet.GetMessageID());
	if(size > util::get_max_unsigned_integer(std::numeric_limits<uint32_t>::digits - 1))
		return false;
	constexpr auto maxSizeBits = std::numeric_limits<uint16_t>::digits - 1;
	if(size > util::get_max_unsigned_integer(maxSizeBits))
		header->Write<uint32_t>((size << 1) | 1);
	else
		header->Write<uint16_t>((size << 1) & ~1);
	return true;
}
bool NWMIO::GetPacketHeaderData(NetPacket &packet, DataStream &header) { return write_packet_header_data(header, packet, packet->GetSize()); }

void NWMIO::SendNextFragment(const NWMEndpoint &ep)
{
	auto writeSize = umath::min(m_writeSizeLeft, static_cast<std::size_t>(NWM_MAX_PACKET_SIZE));
	auto packetSize = m_writeItem->packet->GetSize();
	auto szWritten = packetSize - m_writeSizeLeft;
	std::vector<nwm::MutableBuffer> buffers = {boost::asio::buffer(m_writeItem->header->GetData(), m_writeItem->header->GetSize()), boost::asio::buffer(m_writeItem->packet->GetData() + szWritten, writeSize)};
	auto ptr = shared_from_this();
	AsyncWrite(m_writeItem->endPoint, buffers, [ptr, ep](const nwm::ErrorCode &error, std::size_t bytes) { static_cast<NWMIO *>(ptr.get())->HandleWriteBody(ep, error, bytes); });
}

void NWMIO::SendNextPacket()
{
	if(IsTerminated() || m_packets.empty()) {
#if NWM_VERBOSE >= 2
		std::cout << "[NWM] Unable to send next packet: Session is terminated or no packets to send!" << std::endl;
#endif
		return;
	}
	auto &item = m_packets.front();
	if(item.own == false && IsClosing() && UpdateTermination() == true) {
#if NWM_VERBOSE >= 2
		std::cout << "[NWM] Unable to send next packet: Session is closing or terminated!" << std::endl;
#endif
		return;
	}
	item.header->Resize(NWM_PACKET_HEADER_SIZE);
	if(GetPacketHeaderData(item.packet, item.header) == false) {
#ifdef NWM_VERBOSE
		std::cout << "[NWM] Packet " << item.packet.GetMessageID() << " is too large! (" << util::get_pretty_bytes(item.packet->GetSize()) << ")" << std::endl;
#endif
		m_packets.pop();
		return;
	}

	m_writeItem = std::make_unique<PacketQueueItem>(item);
	m_writeItem->packet.SetTimeActivated(util::clock::to_int(util::clock::get_duration_since_start()));
#ifdef NWM_VERBOSE
	//std::cout<<"Sending message to "<<ep.GetIP()<<std::endl;
#endif
#if NWM_VERBOSE >= 2
	std::cout << "[NWM] SendNextPacket to " << item.endPoint.GetIP() << ": " << item.packet.GetMessageID() << " (" << util::get_pretty_bytes(item.packet->GetSize()) << ")" << std::endl;
#endif
	m_bWriting = true;
	m_packets.pop();
	m_writeSizeLeft = m_writeItem->packet->GetSize();
	SendNextFragment(m_writeItem->endPoint);
}

void NWMIO::HandleWriteBody(NWMEndpoint ep, const nwm::ErrorCode &error, std::size_t sz)
{
	m_bWriting = false;
	if(sz < NWM_PACKET_HEADER_SIZE || !IsConnectionActive() || HandleError(error) == false) {
		ResetWrite();
		return;
	}
	UpdateTermination();
	if(!IsConnectionActive())
		return; // TODO termination has to occur AFTER (Set Variable +do it in Run())
	m_writeSizeLeft -= (sz - m_writeItem->header->GetSize());
	if(m_writeSizeLeft > 0) {
		m_writeItem->header->SetOffset(0);
		if(write_packet_header_data(m_writeItem->header, m_writeItem->packet, m_writeSizeLeft) == false) {
#ifdef NWM_VERBOSE
			std::cout << "[NWM] Packet " << m_writeItem->packet.GetMessageID() << " is too large! (" << util::get_pretty_bytes(m_writeItem->packet->GetSize()) << ")" << std::endl;
#endif
			ResetWrite();
			return;
		}

		m_bWriting = true;
		SendNextFragment(ep);
		return;
	}
	auto writePacket = m_writeItem->packet;
	ResetWrite();

#if NWM_VERBOSE >= 2
	std::cout << "[NWM] HandleWriteBody: " << writePacket.GetMessageID() << " (" << util::get_pretty_bytes(sz) << " / " << util::get_pretty_bytes(writePacket->GetSize()) << ") (" << error.message() << ")" << std::endl;
#endif
	SendNextPacket();
}

void NWMIO::AsyncReceive(uint32_t headerSize)
{
	if(!IsConnectionActive())
		return;
	//m_readPacket->SetOffset(0);
	auto szRead = m_readSizeLeft;
	auto offset = m_readPacket->GetSize() - szRead;
#ifndef NWM_SEGMENT_LARGE_PACKAGES
	m_readSizeLeft = 0;
#else
	if(szRead > NWM_MAX_PACKET_SIZE) {
		szRead = NWM_MAX_PACKET_SIZE;
		m_readSizeLeft -= NWM_MAX_PACKET_SIZE;
	}
	else
		m_readSizeLeft = 0;
#endif
	auto ptr = shared_from_this();
	std::vector<nwm::MutableBuffer> buffers;
	buffers.reserve(2);
	if(IsUDP())
		buffers.push_back(boost::asio::buffer(m_header.get(), headerSize)); // We'll have to read the header again
	buffers.push_back(boost::asio::buffer(m_readPacket->GetData() + offset, szRead));
	AsyncRead(buffers, [ptr](const nwm::ErrorCode &error, std::size_t bytes) { static_cast<NWMIO *>(ptr.get())->HandleReadBody(error, bytes); });
}

void NWMIO::AcceptNextFragment()
{
	if(!IsConnectionActive() || IsClosing())
		return;
	auto ptr = shared_from_this();
	// We must only peek when using the UDP protocol, because the body is part of the same message and would be
	// discarded otherwise
	AsyncRead(
	  {boost::asio::buffer(m_header.get(), NWM_PACKET_HEADER_SIZE)}, [ptr](const nwm::ErrorCode &error, std::size_t bytes) { static_cast<NWMIO *>(ptr.get())->HandleReadHeader(error, bytes); }, IsUDP());
}

void NWMIO::ResetWrite()
{
	m_writeItem = nullptr;
	m_writeSizeLeft = 0;
	m_bWriting = false;
}

void NWMIO::ResetRead()
{
	m_readSizeLeft = 0;
	m_bReading = false;
}

void NWMIO::ReadExtendedHeader()
{
	auto ptr = shared_from_this();
	if(IsUDP()) {
		// Re-read entire header
		AsyncRead(
		  {boost::asio::buffer(m_header.get(), NWM_PACKET_HEADER_EXTENDED_SIZE)}, [ptr](const nwm::ErrorCode &error, std::size_t bytes) { static_cast<NWMIO *>(ptr.get())->HandleReadHeader(error, bytes); }, true);
	}
	else {
		// Only read remaining extended header
		AsyncRead({boost::asio::buffer(m_header.get() + NWM_PACKET_HEADER_SIZE, NWM_PACKET_HEADER_EXTENDED_SIZE - NWM_PACKET_HEADER_SIZE)}, [ptr](const nwm::ErrorCode &error, std::size_t bytes) { static_cast<NWMIO *>(ptr.get())->HandleReadHeader(error, NWM_PACKET_HEADER_SIZE + bytes); });
	}
}

void NWMIO::DiscardUDPData(uint32_t size, const std::function<void(void)> &f)
{
	if(IsTCP()) {
		f();
		return;
	}
	auto data = std::make_shared<std::vector<uint8_t>>(size);
	AsyncRead({boost::asio::buffer(data.get(), data->size())}, [data, f](const nwm::ErrorCode &error, std::size_t bytes) { f(); });
}

void NWMIO::HandleReadHeader(const nwm::ErrorCode &err, std::size_t bytes)
{
	if(!IsConnectionActive() || HandleError(err) == false)
		return;
	m_tLastMessage = util::Clock::now();
	auto bExtended = (bytes == NWM_PACKET_HEADER_EXTENDED_SIZE) ? true : false;
	if(bytes != NWM_PACKET_HEADER_SIZE && bExtended == false) {
#ifdef NWM_VERBOSE
		std::cout << "[NWM] Invalid header!" << std::endl;
#endif
		DiscardUDPData(bytes, [this]() { AcceptNextFragment(); });
		return;
	}
	auto id = *static_cast<uint16_t *>(static_cast<void *>(m_header.get()));
	uint32_t szBody = 0;
	if(bExtended == false) {
		szBody = *static_cast<uint16_t *>(static_cast<void *>(m_header.get() + sizeof(uint16_t)));
		if(szBody & 1) {
			// We need to read additional bytes to get the full header
			ReadExtendedHeader();
			return;
		}
	}
	else {
		szBody = *static_cast<uint32_t *>(static_cast<void *>(m_header.get() + sizeof(uint16_t)));
		szBody &= ~1; // Remove bit which indicates that this is an extended header
	}
	szBody >>= 1;

#if NWM_VERBOSE >= 2
	std::cout << "[NWM] HandleReadHeader: " << id << " (" << util::get_pretty_bytes(bytes) << " / " << util::get_pretty_bytes(szBody) << ") (" << err.message() << ")" << std::endl;
#endif
#ifndef NWM_SEGMENT_LARGE_PACKAGES
	if(szBody > NWM_MAX_PACKET_SIZE) // Something's wrong; Skip this packet
	{
#ifdef NWM_VERBOSE
		std::cout << "[NWM] Packet is too large!" << std::endl;
#endif
		AcceptNextPacket();
		return;
	}
#endif
	if(m_readSizeLeft == 0) // It's a new packet
	{
		m_readPacket = NetPacket(id, szBody);
		m_readSizeLeft = szBody;
		m_readPacket.SetTimeActivated(util::clock::to_int(util::clock::get_duration_since_start()));
	}
	else if(szBody != m_readSizeLeft) // Size has to be the same as what we're expecting, otherwise a packet was most likely lost
	{
		ResetRead();
#ifdef NWM_VERBOSE
		std::cout << "[NWM] Unexpected packet " << id << " with size " << util::get_pretty_bytes(szBody) << "! Discarding..." << std::endl;
#endif
		AcceptNextFragment();
		return;
	}
	m_bReading = true;
	if(szBody == 0 && IsTCP()) // No body; Relay it immediately (For UDP we have to re-read the header, so we'll go the usual way)
	{
		HandleReadBody(err, 0);
		return;
	}
	AsyncReceive(bytes);
}

void NWMIO::HandleReadBody(const nwm::ErrorCode &error, std::size_t sz)
{
	if(m_bReading == false) // We weren't expecting a body?
	{
		AcceptNextFragment();
		return;
	}
	m_bReading = false;
	if(!IsConnectionActive() || HandleError(error) == false)
		return;
	m_tLastMessage = util::Clock::now();
	unsigned int id = m_readPacket.GetMessageID();
#if NWM_VERBOSE >= 2
	std::cout << "[NWM] HandleReadBody: " << id << " (" << util::get_pretty_bytes(sz) << " / " << util::get_pretty_bytes(m_readPacket->GetSize()) << ") (" << error.message() << ")" << std::endl;
#endif
	if(m_readSizeLeft > 0) {
		m_bReading = true;
		AcceptNextFragment();
		return;
	}
	NetPacket inPacket = m_readPacket;
	inPacket->SetOffset(0);
	m_readPacket = NetPacket();
	auto ep = GetRemoteEndPoint().Copy(); // Remote endpoint might change next time any data is received, so we'll have to make a copy
	ScheduleEvent(std::bind(
	  [this, id, ep](NetPacket packet) {
		  if(m_packetHandle != nullptr)
			  m_packetHandle(ep, (NWMIOBase *)this, (unsigned int)id, packet);
	  },
	  inPacket));
	AcceptNextFragment();
}
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
