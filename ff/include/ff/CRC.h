/*
 * WCRC.h
 *
 *  Created on: Aug 2, 2012
 *      Author: ducky
 */

#ifndef FF_CRC_H_
#define FF_CRC_H_

#include <ff/Object.h>
#include <cstdint>
#include <string>
#include <map>

namespace NS_FF {

enum class CrcAlgorithms {
	Undefined,
	Crc8,
	Crc8Cdma2000,
	Crc8Darc,
	Crc8DvbS2,
	Crc8Ebu,
	Crc8ICode,
	Crc8Itu,
	Crc8Maxim,
	Crc8Rohc,
	Crc8Wcdma,
	Crc16CcittFalse,
	Crc16Arc,
	Crc16AugCcitt,
	Crc16Buypass,
	Crc16Cdma2000,
	Crc16Dds110,
	Crc16DectR,
	Crc16DectX,
	Crc16Dnp,
	Crc16En13757,
	Crc16Genibus,
	Crc16Maxim,
	Crc16Mcrf4Xx,
	Crc16Riello,
	Crc16T10Dif,
	Crc16Teledisk,
	Crc16Tms37157,
	Crc16Usb,
	CrcA,
	Crc16Kermit,
	Crc16Modbus,
	Crc16X25,
	Crc16Xmodem,
	Crc32,
	Crc32Bzip2,
	Crc32C,
	Crc32D,
	Crc32Jamcrc,
	Crc32Mpeg2,
	Crc32Posix,
	Crc32Q,
	Crc32Xfer,
	Crc40Gsm,
	Crc64,
	Crc64We,
	Crc64Xz,
	Crc24,
	Crc24FlexrayA,
	Crc24FlexrayB,
	Crc31Philips,
	Crc10,
	Crc10Cdma2000,
	Crc11,
	Crc123Gpp,
	Crc12Cdma2000,
	Crc12Dect,
	Crc13Bbc,
	Crc14Darc,
	Crc15,
	Crc15Mpt1327
};

class FFDLL CrcParamInfo {
public:
	CrcParamInfo(const std::string& name, int hashSize, uint64_t poly,
			uint64_t init, bool refIn, bool refOut, uint64_t xorOut,
			uint64_t check);

	uint64_t getCheck() const;
	int getHashSize() const;
	uint64_t getInit() const;
	const std::string& getName() const;
	uint64_t getPoly() const;
	bool isRefIn() const;
	bool isRefOut() const;
	uint64_t getXorOut() const;

private:
	std::string m_name;
	int m_hashSize;
	uint64_t m_poly;
	uint64_t m_init;
	bool m_refIn;
	bool m_refOut;
	uint64_t m_xorOut;
	uint64_t m_check;
};

typedef std::map<CrcAlgorithms, CrcParamInfo> CrcParamInfos;

class FFDLL CrcParams {
public:
	~CrcParams() = default;

	static CrcParams& instance() {
		static CrcParams crcParams;
		return crcParams;
	}

	const CrcParamInfo* getCrcParamInfo(CrcAlgorithms crcAlgorithms) const;
	const CrcParamInfos& getCrcParamInfos() const;

private:
	CrcParams();
	CrcParams(const CrcParams&) = delete;
	CrcParams& operator=(const CrcParams&) = delete;

	CrcParamInfos m_crcParamInfos;
};

class CrcCalculator {
public:
	CrcCalculator(CrcAlgorithms crcAlgorithms);
	uint64_t calc(const void* buf, int offset, int length);
	const CrcParamInfo* getCrcParamInfo() const;

private:
	const CrcParamInfo* m_crcParamInfo;
	unsigned char m_hashSize;
	uint64_t m_mask;
	uint64_t m_table[256];
	void createTable();
	uint64_t createTableEntry(int index);
	uint64_t computeCrc(uint64_t init, const void* buf, int offset, int length);
};

#if 0
class FFDLL CRC {
public:
	static unsigned short CRC16(const void* buf, unsigned int len);
	static unsigned short CRC16_X25(const void *buf, unsigned int len);

private:
	static unsigned short CRC16_1(const void *buf, unsigned int len);
	static unsigned short CRC16_2(const void *buf, unsigned int len);
	static unsigned short CRC16_3(const void *buf, unsigned int len);

	CRC();
	virtual ~CRC();
};
#endif

} /* namespace NS_FF */

#endif /* FF_CRC_H_ */
