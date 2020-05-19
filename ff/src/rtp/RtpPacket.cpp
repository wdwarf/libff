/*
 * RtpPacket.h
 *
 *  Created on: 2020-05-19
 *      Author: liyawu
 */

#include <ff/RtpPacket.h>
#include <ff/Socket.h>

#define RTP_VERSION 2
#define RTP_MAXCSRCS 15
#define RTP_MINPACKETSIZE 600
#define RTP_DEFAULTPACKETSIZE 1400
#define RTP_PROBATIONCOUNT 2
#define RTP_MAXPRIVITEMS 256
#define RTP_SENDERTIMEOUTMULTIPLIER 2
#define RTP_BYETIMEOUTMULTIPLIER 1
#define RTP_MEMBERTIMEOUTMULTIPLIER 5
#define RTP_COLLISIONTIMEOUTMULTIPLIER 10
#define RTP_NOTETTIMEOUTMULTIPLIER 25
#define RTP_DEFAULTSESSIONBANDWIDTH 10000.0

namespace NS_FF
{
    bool RtpPacket::parse(const void *buf, uint32_t len)
    {
        this->clear();

        if (len < sizeof(RtpHeader))
            return false;

        const uint8_t *packetBytes = (const uint8_t *)buf;
        RtpHeader *hdr = (RtpHeader *)packetBytes;

        if (hdr->version != RTP_VERSION)
            return false;

        if (0 != hdr->marker)
        {
            if (hdr->payloadType == (int(RtcpType::SR) & 127))
                return false;
            if (hdr->payloadType == (int(RtcpType::RR) & 127))
                return false;
        }

        auto payloadOffset = sizeof(RtpHeader) + (int)(hdr->csrcCount * sizeof(uint32_t));
        int32_t paddingBytes = 0;
        if (0 != hdr->padding)
        {
            paddingBytes = (int)packetBytes[len - 1];
            if (paddingBytes <= 0)
                return false;
        }

        RtpExtensionHeader *extHdr = nullptr;
        if (0 != hdr->extension)
        {
            extHdr = (RtpExtensionHeader *)(packetBytes + payloadOffset);
            payloadOffset += sizeof(RtpExtensionHeader);

            uint16_t exthdrlen = ntohs(extHdr->length);
            payloadOffset += ((int)exthdrlen) * sizeof(uint32_t);
        }

        auto payloadLength = len - paddingBytes - payloadOffset;
        if (payloadLength < 0)
            return false;

        this->setData(buf, len);

        return true;
    }

    RtpHeader *RtpPacket::getRtpHeader() const
    {
        if (this->isEmpty())
            return nullptr;
        return (RtpHeader *)this->getData();
    }

    uint32_t RtpPacket::getPayloadOffset() const
    {

        RtpHeader *hdr = this->getRtpHeader();
        if (nullptr == hdr)
            return 0;

        auto payloadOffset = sizeof(RtpHeader) + (int)(hdr->csrcCount * sizeof(uint32_t));

        RtpExtensionHeader *extHdr = nullptr;
        if (0 != hdr->extension)
        {
            extHdr = (RtpExtensionHeader *)(this->getData() + payloadOffset);
            uint16_t exthdrlen = ntohs(extHdr->length);
            payloadOffset += sizeof(RtpExtensionHeader) + ((int)exthdrlen) * sizeof(uint32_t);
        }

        return payloadOffset;
    }

    RtpExtensionHeader *RtpPacket::getRtpExtensionHeader() const
    {
        RtpHeader *hdr = this->getRtpHeader();
        if (nullptr == hdr)
            return nullptr;

        if (0 == hdr->extension)
            return nullptr;

        auto payloadOffset = sizeof(RtpHeader) + (int)(hdr->csrcCount * sizeof(uint32_t));

        return (RtpExtensionHeader *)(this->getData() + payloadOffset);
    }

    const uint8_t *RtpPacket::getPayload() const
    {
        auto hdr = this->getRtpHeader();
        if (nullptr == hdr)
            return nullptr;

        auto payloadOffset = this->getPayloadOffset();
        return this->getData() + payloadOffset;
    }

    uint32_t RtpPacket::getPayloadLength() const
    {
        auto hdr = this->getRtpHeader();
        if (nullptr == hdr)
            return 0;

        int32_t paddingBytes = (0 != hdr->padding) ? ((int)this->getData()[this->getSize() - 1]) : 0;
        auto payloadOffset = this->getPayloadOffset();
        return this->getSize() - paddingBytes - payloadOffset;
    }

} // namespace NS_FF
