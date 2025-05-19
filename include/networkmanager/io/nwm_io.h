/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_IO_H__
#define __NWM_IO_H__

#include "nwm_io_base.h"
#include "nwm_io_header.h"
#include "networkmanager/wrappers/nwm_error_code.hpp"
#include "networkmanager/wrappers/nwm_mutable_buffer.hpp"
#include <functional>

namespace nwm {
	enum class Protocol : uint32_t;
};
class NWMIO : public NWMErrorHandle, public NWMIOHeader, public NWMIOBase {
  protected:
	struct PacketQueueItem {
		PacketQueueItem(const NetPacket &p, const NWMEndpoint &ep, bool bOwn) : packet(p), own(bOwn), endPoint(ep) {}
		NetPacket packet;
		DataStream header;
		NWMEndpoint endPoint;
		bool own;
	};
  private:
	void SendPacket(const PacketQueueItem &item);
	void ResetRead();
	void ResetWrite();
	void ReadExtendedHeader();
	void DiscardUDPData(uint32_t size, const std::function<void(void)> &f);
  protected:
	NWMIO();
	virtual ~NWMIO();
	std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> m_packetHandle;
	bool m_bWriting;
	bool m_bReading;
	bool m_bReady;
	std::queue<PacketQueueItem> m_initPackets;
	std::queue<PacketQueueItem> m_packets;
	NetPacket m_readPacket;
	std::unique_ptr<PacketQueueItem> m_writeItem = nullptr;
	size_t m_readSizeLeft;
	size_t m_writeSizeLeft;
	virtual void Terminate() override;
	bool IsWriting();
	bool IsReading();
	bool GetPacketHeaderData(NetPacket &packet, DataStream &header);
	void ClearPackets();
	void AsyncReceive(uint32_t headerSize);

	void AcceptNextFragment();
	void SendNextPacket();
	void SendNextFragment(const NWMEndpoint &ep);
	virtual void OnError(const nwm::ErrorCode &err) override;
	virtual bool ShouldTerminate() override;
	virtual void HandleReadHeader(const nwm::ErrorCode &err, std::size_t bytes);
	void HandleReadBody(const nwm::ErrorCode &error, std::size_t bytes);
	void HandleWriteBody(NWMEndpoint ep, const nwm::ErrorCode &error, std::size_t bytes);

	virtual void AsyncWrite(const NWMEndpoint &ep, const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f) = 0;
	virtual void AsyncRead(const std::vector<nwm::MutableBuffer> &buffers, const std::function<void(const nwm::ErrorCode &, std::size_t)> &f, bool bPeek = false) = 0;

	virtual bool HandleError(const nwm::ErrorCode &error) override;
	virtual void SendPacket(const NetPacket &packet, const NWMEndpoint &ep, bool bOwn = false);
	void SendPacket(const NetPacket &packet, bool bOwn);
  public:
	void SetPacketHandle(const std::function<void(const NWMEndpoint &, NWMIOBase *, unsigned int, NetPacket &)> &cbPacket);
	virtual void SendPacket(const NetPacket &packet);
	virtual void SetReady() override;
	virtual bool IsReady() const override;
	virtual nwm::Protocol GetProtocol() const = 0;
	bool IsTCP() const;
	bool IsUDP() const;
};

#endif
