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

    uint8_t RtpHeader::getCsrcCount() const
    {
        return csrcCount;
    }

    void RtpHeader::setCsrcCount(uint8_t csrcCount)
    {
        this->csrcCount = csrcCount;
    }

    uint8_t RtpHeader::getExtension() const
    {
        return extension;
    }

    void RtpHeader::setExtension(uint8_t extension)
    {
        this->extension = extension;
    }

    uint8_t RtpHeader::getMarker() const
    {
        return marker;
    }

    void RtpHeader::setMarker(uint8_t marker)
    {
        this->marker = marker;
    }

    uint8_t RtpHeader::getPadding() const
    {
        return padding;
    }

    void RtpHeader::setPadding(uint8_t padding)
    {
        this->padding = padding;
    }

    uint8_t RtpHeader::getPayloadType() const
    {
        return payloadType;
    }

    void RtpHeader::setPayloadType(uint8_t payloadType)
    {
        this->payloadType = payloadType;
    }

    uint16_t RtpHeader::getSequenceNumber() const
    {
        return ntohs(sequenceNumber);
    }

    void RtpHeader::setSequenceNumber(uint16_t sequenceNumber)
    {
        this->sequenceNumber = htons(sequenceNumber);
    }

    uint32_t RtpHeader::getSsrc() const
    {
        return ntohl(ssrc);
    }

    void RtpHeader::setSsrc(uint32_t ssrc)
    {
        this->ssrc = htonl(ssrc);
    }

    uint32_t RtpHeader::getTimestamp() const
    {
        return ntohl(timestamp);
    }

    void RtpHeader::setTimestamp(uint32_t timestamp)
    {
        this->timestamp = htonl(timestamp);
    }

    uint8_t RtpHeader::getVersion() const
    {
        return version;
    }

    void RtpHeader::setVersion(uint8_t version)
    {
        this->version = version;
    }

    uint16_t RtpExtensionHeader::getExtid() const
    {
        return ntohs(extid);
    }

    void RtpExtensionHeader::setExtid(uint16_t extid)
    {
        this->extid = htons(extid);
    }

    uint16_t RtpExtensionHeader::getLength() const
    {
        return ntohs(length);
    }

    void RtpExtensionHeader::setLength(uint16_t length)
    {
        this->length = htons(length);
    }

    bool RtpPacket::parse(const void *buf, uint32_t len)
    {
        this->clear();

        if (len < sizeof(RtpHeader))
            return false;

        const uint8_t *packetBytes = (const uint8_t *)buf;
        RtpHeader *hdr = (RtpHeader *)packetBytes;

        if (hdr->getVersion() != RTP_VERSION)
            return false;

        if (0 != hdr->getMarker())
        {
            if (hdr->getPayloadType() == (int(RtcpType::SR) & 127))
                return false;
            if (hdr->getPayloadType() == (int(RtcpType::RR) & 127))
                return false;
        }

        auto payloadOffset = sizeof(RtpHeader) + (int)(hdr->getCsrcCount() * sizeof(uint32_t));
        int32_t paddingBytes = 0;
        if (0 != hdr->getPadding())
        {
            paddingBytes = (int)packetBytes[len - 1];
            if (paddingBytes <= 0)
                return false;
        }

        RtpExtensionHeader *extHdr = nullptr;
        if (0 != hdr->getExtension())
        {
            extHdr = (RtpExtensionHeader *)(packetBytes + payloadOffset);
            payloadOffset += sizeof(RtpExtensionHeader);

            uint16_t exthdrlen = extHdr->getLength();
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

        auto payloadOffset = sizeof(RtpHeader) + (int)(hdr->getCsrcCount() * sizeof(uint32_t));

        RtpExtensionHeader *extHdr = nullptr;
        if (0 != hdr->getExtension())
        {
            extHdr = (RtpExtensionHeader *)(this->getData() + payloadOffset);
            uint16_t exthdrlen = extHdr->getLength();
            payloadOffset += sizeof(RtpExtensionHeader) + ((int)exthdrlen) * sizeof(uint32_t);
        }

        return payloadOffset;
    }

    RtpExtensionHeader *RtpPacket::getRtpExtensionHeader() const
    {
        RtpHeader *hdr = this->getRtpHeader();
        if (nullptr == hdr)
            return nullptr;

        if (0 == hdr->getExtension())
            return nullptr;

        auto payloadOffset = sizeof(RtpHeader) + (int)(hdr->getCsrcCount() * sizeof(uint32_t));

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

        int32_t paddingBytes = (0 != hdr->getPadding() ) ? ((int)this->getData()[this->getSize() - 1]) : 0;
        auto payloadOffset = this->getPayloadOffset();
        return this->getSize() - paddingBytes - payloadOffset;
    }

    uint32_t RtpPacket::getCSRC(int n) const
    {
        auto hdr = this->getRtpHeader();
        if (nullptr == hdr)
            return 0;

        if (n >= hdr->getCsrcCount())
            return 0;

        uint8_t *csrcpos = this->getData() + sizeof(RtpHeader) + n * sizeof(uint32_t);
        uint32_t *csrcvalNbo = (uint32_t *)csrcpos;
        return ntohl(*csrcvalNbo);
    }

} // namespace NS_FF
