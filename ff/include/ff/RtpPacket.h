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

        private:
        uint32_t getPayloadOffset() const;

    }; /* class RtpPacket */

} // namespace NS_FF

#endif