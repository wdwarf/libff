/*
 * CRC.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: ducky
 */

#include <ff/CRC.h>
#include <ff/Buffer.h>
#include <iostream>
using namespace std;

namespace NS_FF {

#if 0

CRC::CRC() {
	//do nothing...
}

CRC::~CRC() {
	//do nothing...
}

unsigned short CRC::CRC16_X25(const void *buf, unsigned int bufLen) {
	auto pBuf = (const unsigned char *) buf;
	unsigned int i = 0, j = 0, lsb = 0;
	uint16_t crcResult = 0xFFFF;
	for (i = 0; i < bufLen; i++) {
		crcResult ^= pBuf[i];
		for (j = 0; j < 8; j++) {
			lsb = crcResult & 0x0001;
			crcResult >>= 1;
			if (lsb == 1) {
				crcResult ^= 0x8408;
			}
		}
	}
	crcResult ^= 0xffff;
	return crcResult;
}

//查表法计算CRC16 方法1  (CRC16余式表是按0x11021多项式:x16+x12+x2+1 )
unsigned short CRC::CRC16(const void* buf, unsigned int len) {
	const unsigned char* ptr = (const unsigned char*) buf;
	unsigned short CRC16;
	unsigned char High_8bits;
	unsigned short CRC_Remainder_Table[256] = {0x0000, 0x1021, 0x2042, 0x3063,
		0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
		0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252,
		0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a,
		0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
		0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509,
		0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630,
		0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
		0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7,
		0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af,
		0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
		0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e,
		0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5,
		0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
		0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4,
		0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc,
		0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
		0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3,
		0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da,
		0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
		0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589,
		0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481,
		0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
		0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0,
		0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f,
		0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
		0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e,
		0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16,
		0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
		0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45,
		0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c,
		0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
		0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

	CRC16 = 0;
	while (len-- != 0) {
		High_8bits = (unsigned char) (CRC16 >> 8);
		CRC16 <<= 8;
		CRC16 ^= CRC_Remainder_Table[High_8bits ^ *ptr];
		ptr++;
	}
	return (CRC16);
}

//-----------------------------------------------------------------------------------------------

//查表法计算CRC16 方法2  (CRC16余式表是按0x11021多项式:x16+x12+x2+1 )
unsigned short CRC::CRC16_1(const void* buf, unsigned int len) {
	const unsigned char* ptr = (const unsigned char*) buf;
	unsigned short CRC16;
	unsigned char High_8bits;
	unsigned short CRC_Remainder_Table[16] = {0x0000, 0x1021, 0x2042, 0x3063,
		0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
		0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,};

	CRC16 = 0;
	while (len-- != 0) {
		High_8bits = ((unsigned char) (CRC16 / 256)) / 16;
		CRC16 <<= 4;
		CRC16 ^= CRC_Remainder_Table[High_8bits ^ (*ptr / 16)];
		High_8bits = ((unsigned char) (CRC16 / 256)) / 16;
		CRC16 <<= 4;
		CRC16 ^= CRC_Remainder_Table[High_8bits ^ (*ptr & 0x0f)];
		ptr++;
	}
	return (CRC16);
}

//-----------------------------------------------------------------------------------------------

//查表法计算CRC16 方法3 (CRC余式表是按0x11021多项式:x16+x12+x2+1 )
unsigned short CRC::CRC16_2(const void* buf, unsigned int len) {
	const unsigned char* ptr = (const unsigned char*) buf;
	unsigned char i;
	unsigned short CRC16 = 0;
	while (len-- != 0) {
		for (i = 0x80; i != 0; i /= 2) {
			if ((CRC16 & 0x8000) != 0) {
				CRC16 *= 2;
				CRC16 ^= 0x1021;
			} /* 余式CRC乘以2再求CRC */
			else
			CRC16 *= 2;

			if ((*ptr & i) != 0)
			CRC16 ^= 0x1021; /* 再加上本位的CRC */
		}
		ptr++;
	}
	return (CRC16);
}

//-----------------------------------------------------------------------------------------------

//查表法计算CRC16 方法4  (CRC余式表是按0x11021多项式:x16+x12+x2+1 )
unsigned short CRC::CRC16_3(const void* buf, unsigned int len) {
	const unsigned char* ptr = (const unsigned char*) buf;
	unsigned short CRC_Remainder_Table[256] = {0x0000, 0x1021, 0x2042, 0x3063,
		0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
		0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252,
		0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a,
		0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
		0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509,
		0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630,
		0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
		0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7,
		0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af,
		0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
		0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e,
		0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5,
		0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
		0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4,
		0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc,
		0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
		0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3,
		0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da,
		0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
		0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb, 0x95a8, 0x8589,
		0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481,
		0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
		0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0,
		0x6657, 0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f,
		0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
		0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e,
		0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16,
		0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
		0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45,
		0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c,
		0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
		0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

	unsigned int i;
	unsigned short CRC16 = 0;

	for (i = 0; i < len; i++)
	CRC16 = (CRC16 << 8)
	^ (unsigned short) CRC_Remainder_Table[(CRC16 >> 8) ^ *ptr++];
	return CRC16;

}

#endif

CrcParamInfo::CrcParamInfo(const std::string& name, int hashSize, uint64_t poly,
		uint64_t init, bool refIn, bool refOut, uint64_t xorOut, uint64_t check) :
		m_name(name), m_hashSize(hashSize), m_poly(poly), m_init(init), m_refIn(
				refIn), m_refOut(refOut), m_xorOut(xorOut), m_check(check) {
}

uint64_t CrcParamInfo::getCheck() const {
	return m_check;
}

int CrcParamInfo::getHashSize() const {
	return m_hashSize;
}

uint64_t CrcParamInfo::getInit() const {
	return m_init;
}

const std::string& CrcParamInfo::getName() const {
	return m_name;
}

uint64_t CrcParamInfo::getPoly() const {
	return m_poly;
}

bool CrcParamInfo::isRefIn() const {
	return m_refIn;
}

bool CrcParamInfo::isRefOut() const {
	return m_refOut;
}

uint64_t CrcParamInfo::getXorOut() const {
	return m_xorOut;
}

CrcParams::CrcParams() {
	this->m_crcParamInfos =
	{
		//CRC-8
		{	CrcAlgorithms::Crc8 , CrcParamInfo("CRC-8", 8, 0x7, 0x0, false, false, 0x0, 0xF4)},
		{	CrcAlgorithms::Crc8Cdma2000 , CrcParamInfo("CRC-8/CDMA2000", 8, 0x9B, 0xFF, false, false, 0x0, 0xDA)},
		{	CrcAlgorithms::Crc8Darc , CrcParamInfo("CRC-8/DARC", 8, 0x39, 0x0, true, true, 0x0, 0x15)},
		{	CrcAlgorithms::Crc8DvbS2 , CrcParamInfo("CRC-8/DVB-S2", 8, 0xD5, 0x0, false, false, 0x0, 0xBC)},
		{	CrcAlgorithms::Crc8Ebu , CrcParamInfo("CRC-8/EBU", 8, 0x1D, 0xFF, true, true, 0x0, 0x97)},
		{	CrcAlgorithms::Crc8ICode , CrcParamInfo("CRC-8/I-CODE", 8, 0x1D, 0xFD, false, false, 0x0, 0x7E)},
		{	CrcAlgorithms::Crc8Itu , CrcParamInfo("CRC-8/ITU", 8, 0x7, 0x0, false, false, 0x55, 0xA1)},
		{	CrcAlgorithms::Crc8Maxim , CrcParamInfo("CRC-8/MAXIM", 8, 0x31, 0x0, true, true, 0x0, 0xA1)},
		{	CrcAlgorithms::Crc8Rohc , CrcParamInfo("CRC-8/ROHC", 8, 0x7, 0xFF, true, true, 0x0, 0xD0)},
		{	CrcAlgorithms::Crc8Wcdma , CrcParamInfo("CRC-8/WCDMA", 8, 0x9B, 0x0, true, true, 0x0, 0x25)},

		//CRC-10
		{	CrcAlgorithms::Crc10 , CrcParamInfo("CRC-10", 10, 0x233, 0x0, false, false, 0x0, 0x199)},
		{	CrcAlgorithms::Crc10Cdma2000 , CrcParamInfo("CRC-10/CDMA2000", 10, 0x3D9, 0x3FF, false, false, 0x0, 0x233)},

		//CRC-11
		{	CrcAlgorithms::Crc11 , CrcParamInfo("CRC-11", 11, 0x385, 0x1A, false, false, 0x0, 0x5A3)},

		//CRC-12
		{	CrcAlgorithms::Crc123Gpp , CrcParamInfo("CRC-12/3GPP", 12, 0x80F, 0x0, false, true, 0x0, 0xDAF)},
		{	CrcAlgorithms::Crc12Cdma2000 , CrcParamInfo("CRC-12/CDMA2000", 12, 0xF13, 0xFFF, false, false, 0x0, 0xD4D)},
		{	CrcAlgorithms::Crc12Dect , CrcParamInfo("CRC-12/DECT", 12, 0x80F, 0x0, false, false, 0x0, 0xF5B)},

		//CRC-13
		{	CrcAlgorithms::Crc13Bbc , CrcParamInfo("CRC-13/BBC", 13, 0x1CF5, 0x0, false, false, 0x0, 0x4FA)},

		//CRC-14
		{	CrcAlgorithms::Crc14Darc , CrcParamInfo("CRC-14/DARC", 14, 0x805, 0x0, true, true, 0x0, 0x82D)},

		//CRC-15
		{	CrcAlgorithms::Crc15 , CrcParamInfo("CRC-15", 15, 0x4599, 0x0, false, false, 0x0, 0x59E)},
		{	CrcAlgorithms::Crc15Mpt1327 , CrcParamInfo("CRC-15/MPT1327", 15, 0x6815, 0x0, false, false, 0x1, 0x2566)},

		//CRC-16
		{	CrcAlgorithms::Crc16CcittFalse, CrcParamInfo("CRC-16/CCITT-FALSE", 16, 0x1021, 0xFFFF, false, false, 0x0, 0x29B1)},
		{	CrcAlgorithms::Crc16Arc , CrcParamInfo("CRC-16/ARC", 16, 0x8005, 0x0, true, true, 0x0, 0xBB3D)},
		{	CrcAlgorithms::Crc16AugCcitt , CrcParamInfo("CRC-16/AUG-CCITT", 16, 0x1021, 0x1D0F, false, false, 0x0, 0xE5CC)},
		{	CrcAlgorithms::Crc16Buypass , CrcParamInfo("CRC-16/BUYPASS", 16, 0x8005, 0x0, false, false, 0x0, 0xFEE8)},
		{	CrcAlgorithms::Crc16Cdma2000 , CrcParamInfo("CRC-16/CDMA2000", 16, 0xC867, 0xFFFF, false, false, 0x0, 0x4C06)},
		{	CrcAlgorithms::Crc16Dds110 , CrcParamInfo("CRC-16/DDS-110", 16, 0x8005, 0x800D, false, false, 0x0, 0x9ECF)},
		{	CrcAlgorithms::Crc16DectR , CrcParamInfo("CRC-16/DECT-R", 16, 0x589, 0x0, false, false, 0x1, 0x7E)},
		{	CrcAlgorithms::Crc16DectX , CrcParamInfo("CRC-16/DECT-X", 16, 0x589, 0x0, false, false, 0x0, 0x7F)},
		{	CrcAlgorithms::Crc16Dnp , CrcParamInfo("CRC-16/DNP", 16, 0x3D65, 0x0, true, true, 0xFFFF, 0xEA82)},
		{	CrcAlgorithms::Crc16En13757 , CrcParamInfo("CRC-16/EN-13757", 16, 0x3D65, 0x0, false, false, 0xFFFF, 0xC2B7)},
		{	CrcAlgorithms::Crc16Genibus , CrcParamInfo("CRC-16/GENIBUS", 16, 0x1021, 0xFFFF, false, false, 0xFFFF, 0xD64E)},
		{	CrcAlgorithms::Crc16Maxim , CrcParamInfo("CRC-16/MAXIM", 16, 0x8005, 0x0, true, true, 0xFFFF, 0x44C2)},
		{	CrcAlgorithms::Crc16Mcrf4Xx , CrcParamInfo("CRC-16/MCRF4XX", 16, 0x1021, 0xFFFF, true, true, 0x0, 0x6F91)},
		{	CrcAlgorithms::Crc16Riello , CrcParamInfo("CRC-16/RIELLO", 16, 0x1021, 0xB2AA, true, true, 0x0, 0x63D0)},
		{	CrcAlgorithms::Crc16T10Dif , CrcParamInfo("CRC-16/T10-DIF", 16, 0x8BB7, 0x0, false, false, 0x0, 0xD0DB)},
		{	CrcAlgorithms::Crc16Teledisk , CrcParamInfo("CRC-16/TELEDISK", 16, 0xA097, 0x0, false, false, 0x0, 0xFB3)},
		{	CrcAlgorithms::Crc16Tms37157 , CrcParamInfo("CRC-16/TMS37157", 16, 0x1021, 0x89EC, true, true, 0x0, 0x26B1)},
		{	CrcAlgorithms::Crc16Usb , CrcParamInfo("CRC-16/USB", 16, 0x8005, 0xFFFF, true, true, 0xFFFF, 0xB4C8)},
		{	CrcAlgorithms::CrcA , CrcParamInfo("CRC-A", 16, 0x1021, 0xC6C6, true, true, 0x0, 0xBF05)},
		{	CrcAlgorithms::Crc16Kermit , CrcParamInfo("CRC-16/KERMIT", 16, 0x1021, 0x0, true, true, 0x0, 0x2189)},
		{	CrcAlgorithms::Crc16Modbus , CrcParamInfo("CRC-16/MODBUS", 16, 0x8005, 0xFFFF, true, true, 0x0, 0x4B37)},
		{	CrcAlgorithms::Crc16X25 , CrcParamInfo("CRC-16/X-25", 16, 0x1021, 0xFFFF, true, true, 0xFFFF, 0x906E)},
		{	CrcAlgorithms::Crc16Xmodem , CrcParamInfo("CRC-16/XMODEM", 16, 0x1021, 0x0, false, false, 0x0, 0x31C3)},

		//CRC-24
		{	CrcAlgorithms::Crc24 , CrcParamInfo("CRC-24", 24, 0x864CFB, 0xB704CE, false, false, 0x0, 0x21CF02)},
		{	CrcAlgorithms::Crc24FlexrayA , CrcParamInfo("CRC-24/FLEXRAY-A", 24, 0x5D6DCB, 0xFEDCBA, false, false, 0x0, 0x7979BD)},
		{	CrcAlgorithms::Crc24FlexrayB , CrcParamInfo("CRC-24/FLEXRAY-B", 24, 0x5D6DCB, 0xABCDEF, false, false, 0x0, 0x1F23B8)},

		//CRC-31
		{	CrcAlgorithms::Crc31Philips , CrcParamInfo("CRC-31/PHILIPS", 31, 0x4C11DB7, 0x7FFFFFFF, false, false, 0x7FFFFFFF, 0xCE9E46C)},

		//CRC-32
		{	CrcAlgorithms::Crc32 , CrcParamInfo("CRC-32", 32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0xFFFFFFFF, 0xCBF43926)},
		{	CrcAlgorithms::Crc32Bzip2 , CrcParamInfo("CRC-32/BZIP2", 32, 0x04C11DB7, 0xFFFFFFFF, false, false, 0xFFFFFFFF, 0xFC891918)},
		{	CrcAlgorithms::Crc32C , CrcParamInfo("CRC-32C", 32, 0x1EDC6F41, 0xFFFFFFFF, true, true, 0xFFFFFFFF, 0xE3069283)},
		{	CrcAlgorithms::Crc32D , CrcParamInfo("CRC-32D", 32, 0xA833982B, 0xFFFFFFFF, true, true, 0xFFFFFFFF, 0x87315576)},
		{	CrcAlgorithms::Crc32Jamcrc , CrcParamInfo("CRC-32/JAMCRC", 32, 0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000, 0x340BC6D9)},
		{	CrcAlgorithms::Crc32Mpeg2 , CrcParamInfo("CRC-32/MPEG-2", 32, 0x04C11DB7, 0xFFFFFFFF, false, false, 0x00000000, 0x0376E6E7)},
		{	CrcAlgorithms::Crc32Posix , CrcParamInfo("CRC-32/POSIX", 32, 0x04C11DB7, 0x00000000, false, false, 0xFFFFFFFF, 0x765E7680)},
		{	CrcAlgorithms::Crc32Q , CrcParamInfo("CRC-32Q", 32, 0x814141AB, 0x00000000, false, false, 0x00000000, 0x3010BF7F)},
		{	CrcAlgorithms::Crc32Xfer , CrcParamInfo("CRC-32/XFER", 32, 0x000000AF, 0x00000000, false, false, 0x00000000, 0xBD0BE338)},

		//CRC-40
		{	CrcAlgorithms::Crc40Gsm , CrcParamInfo("CRC-40/GSM", 40, 0x4820009, 0x0, false, false, 0xFFFFFFFFFF, 0xD4164FC646)},

		//CRC-64
		{	CrcAlgorithms::Crc64 , CrcParamInfo("CRC-64",64, 0x42F0E1EBA9EA3693, 0x00000000, false, false, 0x00000000, 0x6C40DF5F0B497347)},
		{	CrcAlgorithms::Crc64We , CrcParamInfo("CRC-64/WE", 64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, false, false, 0xFFFFFFFFFFFFFFFF, 0x62EC59E3F1A4F00A)},
		{	CrcAlgorithms::Crc64Xz , CrcParamInfo("CRC-64/XZ", 64, 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, true, true, 0xFFFFFFFFFFFFFFFF, 0x995DC9BBDF1939FA)}
	};
}

const CrcParamInfo* CrcParams::getCrcParamInfo(
		CrcAlgorithms crcAlgorithms) const {
	auto it = this->m_crcParamInfos.find(crcAlgorithms);
	if (it == this->m_crcParamInfos.end())
		return nullptr;

	cout << it->second.getName() << endl;
	return &it->second;
}

const CrcParamInfos& CrcParams::getCrcParamInfos() const {
	return m_crcParamInfos;
}

CrcCalculator::CrcCalculator(CrcAlgorithms crcAlgorithms) :
		m_crcParamInfo(CrcParams::instance().getCrcParamInfo(crcAlgorithms)), m_hashSize(
				m_crcParamInfo->getHashSize()), m_mask(0xFFFFFFFFFFFFFFFFull) {
	if (this->m_hashSize < 64) {
		this->m_mask = (1L << this->m_hashSize) - 1;
	}
	this->createTable();
}

void CrcCalculator::createTable() {
	for (size_t i = 0; i < 255; i++)
		this->m_table[i] = createTableEntry(i);
}

static uint64_t ReverseBits(uint64_t ul, int valueLength) {
	uint64_t newValue = 0;

	for (int i = valueLength - 1; i >= 0; i--) {
		newValue |= (ul & 1) << i;
		ul >>= 1;
	}

	return newValue;
}

uint64_t CrcCalculator::createTableEntry(int index) {
	uint64_t r = (uint64_t) index;

	if (this->m_crcParamInfo->isRefIn())
		r = ReverseBits(r, this->m_hashSize);
	else if (this->m_hashSize > 8)
		r <<= (this->m_hashSize - 8);

	uint64_t lastBit = (1L << (this->m_hashSize - 1));

	for (int i = 0; i < 8; i++) {
		if ((r & lastBit) != 0)
			r = ((r << 1) ^ this->m_crcParamInfo->getPoly());
		else
			r <<= 1;
	}

	if (this->m_crcParamInfo->isRefOut())
		r = ReverseBits(r, this->m_hashSize);

	return r & this->m_mask;
}

uint64_t CrcCalculator::calc(const void* data, int offset, int length) {
	uint64_t init =
			this->m_crcParamInfo->isRefOut() ?
					ReverseBits(this->m_crcParamInfo->getInit(),
							this->m_hashSize) :
					this->m_crcParamInfo->getInit();
	uint64_t hash = computeCrc(init, data, offset, length);
	return (hash ^ this->m_crcParamInfo->getXorOut()) & this->m_mask;
}

const CrcParamInfo* CrcCalculator::getCrcParamInfo() const {
	return m_crcParamInfo;
}

uint64_t CrcCalculator::computeCrc(uint64_t init, const void* buf, int offset,
		int length) {
	uint64_t crc = init;

	auto data = (const uint8_t*) buf;
	if (this->m_crcParamInfo->isRefOut()) {
		for (int i = offset; i < offset + length; i++) {
			crc = (this->m_table[(int) ((crc ^ data[i]) & 0xFF)] ^ (crc >> 8));
			crc &= this->m_mask;
		}
	} else {
		int toRight = (this->m_hashSize - 8);
		toRight = toRight < 0 ? 0 : toRight;
		for (int i = offset; i < offset + length; i++) {
			crc = (this->m_table[(int) (((crc >> toRight) ^ data[i]) & 0xFF)]
					^ (crc << 8));
			crc &= this->m_mask;
		}
	}

	return crc;
}

}

