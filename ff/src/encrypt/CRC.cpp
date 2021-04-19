/*
 * CRC.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: ducky
 */

#include <ff/CRC.h>
#include <ff/Buffer.h>

NS_FF_BEG

static uint64_t ReverseBits(uint64_t ul, int valueLength) {
	uint64_t newValue = 0;

	for (int i = valueLength - 1; i >= 0; i--) {
		newValue |= (ul & 1) << i;
		ul >>= 1;
	}

	return newValue;
}

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
	this->initTable();
}

CrcCalculator::CrcCalculator(const CrcParamInfo* crcParamInfo) :
		m_crcParamInfo(crcParamInfo), m_hashSize(m_crcParamInfo->getHashSize()), m_mask(
				0xFFFFFFFFFFFFFFFFull) {
	if (this->m_hashSize < 64) {
		this->m_mask = (1L << this->m_hashSize) - 1;
	}
	this->initTable();
}

void CrcCalculator::initTable() {
	for (size_t i = 0; i < 255; i++)
		this->m_table[i] = this->tableEntry(i);
}

uint64_t CrcCalculator::tableEntry(int index) {
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

uint64_t CrcCalculator::calc(const void* data, int length) const {
	uint64_t init =
			this->m_crcParamInfo->isRefOut() ?
					ReverseBits(this->m_crcParamInfo->getInit(),
							this->m_hashSize) :
					this->m_crcParamInfo->getInit();
	uint64_t hash = this->calcCrc(init, data, length);
	return (hash ^ this->m_crcParamInfo->getXorOut()) & this->m_mask;
}

const CrcParamInfo* CrcCalculator::getCrcParamInfo() const {
	return m_crcParamInfo;
}

uint64_t CrcCalculator::calcCrc(uint64_t init, const void* buf,
		int length) const {
	uint64_t crc = init;

	auto data = (const uint8_t*) buf;
	if (this->m_crcParamInfo->isRefOut()) {
		for (int i = 0; i < length; i++) {
			crc = (this->m_table[((crc ^ data[i]) & 0xFF)] ^ (crc >> 8));
			crc &= this->m_mask;
		}
	} else {
		int toRight = (this->m_hashSize - 8);
		toRight = toRight < 0 ? 0 : toRight;
		for (int i = 0; i < length; i++) {
			crc = (this->m_table[(((crc >> toRight) ^ data[i]) & 0xFF)]
					^ (crc << 8));
			crc &= this->m_mask;
		}
	}

	return crc;
}

}

