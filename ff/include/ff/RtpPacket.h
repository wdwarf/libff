/*
 * RtpPacket.h
 *
 *  Created on: 2020-05-19
 *      Author: liyawu
 */
#ifndef RTP_RTPPACKET_H_
#define RTP_RTPPACKET_H_

#include <ff/ff_config.h>
#include <ff/Buffer.h>

namespace NS_FF
{
    struct RtpHeader
    {
    public:
        uint8_t getCsrcCount() const;
        void setCsrcCount(uint8_t csrcCount);
        uint8_t getExtension() const;
        void setExtension(uint8_t extension);
        uint8_t getMarker() const;
        void setMarker(uint8_t marker);
        uint8_t getPadding() const;
        void setPadding(uint8_t padding);
        uint8_t getPayloadType() const;
        void setPayloadType(uint8_t payloadType);
        uint16_t getSequenceNumber() const;
        void setSequenceNumber(uint16_t sequenceNumber);
        uint32_t getSsrc() const;
        void setSsrc(uint32_t ssrc);
        uint32_t getTimestamp() const;
        void setTimestamp(uint32_t timestamp);
        uint8_t getVersion() const;
        void setVersion(uint8_t version);

private:
        uint8_t csrcCount : 4;
        uint8_t extension : 1;
        uint8_t padding : 1;
        uint8_t version : 2;

        uint8_t payloadType : 7;
        uint8_t marker : 1;

        uint16_t sequenceNumber;
        uint32_t timestamp;
        uint32_t ssrc;
    };

    struct RtpExtensionHeader
    {
    public:
        uint16_t getExtid() const;
        void setExtid(uint16_t extid);
        uint16_t getLength() const;
        void setLength(uint16_t length);

    private:
        uint16_t extid;
        uint16_t length;
    };

    enum class RtcpType
    {
        SR = 200,
        RR = 201,
        SDES = 202,
        BYE = 203,
        APP = 204
    };

    class LIBFF_API RtpPacket : public Buffer
    {
        public:
        bool parse(const void* buf, uint32_t len);

        RtpHeader* getRtpHeader() const;
        RtpExtensionHeader* getRtpExtensionHeader() const;
        const uint8_t* getPayload() const;
        uint32_t getPayloadLength() const;
        uint32_t getCSRC(int num) const;

        private:
        uint32_t getPayloadOffset() const;

    }; /* class RtpPacket */

} // namespace NS_FF

#endif
