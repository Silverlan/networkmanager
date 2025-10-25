// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <functional>
#include <queue>
#include <cinttypes>
#include <memory>

export module pragma.network_manager:io.core;

export import :endpoint;
export import :enums;
export import :error_handle;
export import :io.base;
export import :io.header;
export import :mutable_buffer;
export import pragma.util;

export {
	class NWMIO : public NWMErrorHandle, public NWMIOHeader, public NWMIOBase {
	protected:
		struct PacketQueueItem {
			PacketQueueItem(const NetPacket &p, const NWMEndpoint &ep, bool bOwn) : packet(p), own(bOwn), endPoint(ep) {}
			NetPacket packet;
			util::DataStream header;
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
		bool GetPacketHeaderData(NetPacket &packet, util::DataStream &header);
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
}
