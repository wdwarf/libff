/*
 * CRC.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: ducky
 */

#include <ff/Buffer.h>
#include <ff/CRC.h>

#include <iostream>
#include <mutex>

using namespace std;

NS_FF_BEG

static uint8_t ReflectBitOrderTable[256] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0,
    0x30, 0xB0, 0x70, 0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 0x04, 0x84, 0x44, 0xC4,
    0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC,
    0x3C, 0xBC, 0x7C, 0xFC, 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA,
    0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6,
    0x36, 0xB6, 0x76, 0xF6, 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE, 0x01, 0x81, 0x41, 0xC1,
    0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9,
    0x39, 0xB9, 0x79, 0xF9, 0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD,
    0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
    0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB, 0x07, 0x87, 0x47, 0xC7,
    0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF,
    0x3F, 0xBF, 0x7F, 0xFF};

uint64_t ReflectBits(uint64_t data, uint8_t width) {
  uint64_t reflection = 0;

  for (uint8_t bit = 0; bit < width; ++bit) {
    if (data & 0x01) {
      reflection |= (1ull << ((width - 1) - bit));
    }
    data = (data >> 1);
  }
  return reflection;
}

CrcParameter::CrcParameter(const std::string& name, uint8_t width,
                           uint64_t polynomial, uint64_t initValue,
                           bool reflectIn, bool reflectOut, uint64_t xorOut,
                           uint64_t check)
    : m_name(name),
      m_width(width),
      m_polynomial(polynomial),
      m_initialValue(initValue),
      m_reflectInput(reflectIn),
      m_reflectOutput(reflectOut),
      m_xorOutput(xorOut),
      m_check(check) {
  m_mask = (1ull << m_width) - 1;
  m_highBit = 1ull << (m_width - 1);
  this->updateTable();
}

CrcParameterPtr CrcParameter::Create(const std::string& name, uint8_t width,
                                     uint64_t polynomial, uint64_t initValue,
                                     bool reflectIn, bool reflectOut,
                                     uint64_t xorOut, uint64_t check) {
  return std::make_shared<CrcParameter>(name, width, polynomial, initValue,
                                        reflectIn, reflectOut, xorOut, check);
}

void CrcParameter::updateTable() {
  uint64_t crc = 0;
  int shift = m_width - 8;
  for (uint64_t i = 0; i < 256; i++) {
    crc = i << shift;
    for (uint64_t j = 0; j < 8; j++) {
      if (crc & m_highBit)
        crc = (crc << 1) ^ m_polynomial;
      else
        crc = crc << 1;
    }

    m_eflectBitOrderTable[i] = crc;
  }
}

std::string CrcParameter::name() const { return m_name; }

uint64_t CrcParameter::polynomial() const { return m_polynomial; }

uint64_t CrcParameter::initialValue() const { return m_initialValue; }

bool CrcParameter::reflectInput() const { return m_reflectInput; }

bool CrcParameter::reflectOutput() const { return m_reflectOutput; }

uint64_t CrcParameter::xorOutput() const { return m_xorOutput; }

uint8_t CrcParameter::width() const { return m_width; }

uint64_t CrcParameter::check() const { return m_check; }

uint64_t CrcParameter::mask() const { return m_mask; }

uint64_t CrcParameter::highBit() const { return m_highBit; }

uint64_t CrcParameter::eflectBit(uint8_t v) const {
  return m_eflectBitOrderTable[v];
}

CrcParameterPtr CrcParameter::Create(CrcAlgorithm algo) {
  static std::map<CrcAlgorithm, CrcParameterPtr> algos;
  static std::once_flag flag;
  std::call_once(flag, []() {
    algos = std::map<CrcAlgorithm, CrcParameterPtr>{
        // CRC-8
        {CrcAlgorithm::Crc8,
         CrcParameter::Create("CRC-8", 8, 0x7, 0x0, false, false, 0x0, 0xF4)},
        {CrcAlgorithm::Crc8Cdma2000,
         CrcParameter::Create("CRC-8/CDMA2000", 8, 0x9B, 0xFF, false, false,
                              0x0, 0xDA)},
        {CrcAlgorithm::Crc8Darc,
         CrcParameter::Create("CRC-8/DARC", 8, 0x39, 0x0, true, true, 0x0,
                              0x15)},
        {CrcAlgorithm::Crc8DvbS2,
         CrcParameter::Create("CRC-8/DVB-S2", 8, 0xD5, 0x0, false, false, 0x0,
                              0xBC)},
        {CrcAlgorithm::Crc8Ebu, CrcParameter::Create("CRC-8/EBU", 8, 0x1D, 0xFF,
                                                     true, true, 0x0, 0x97)},
        {CrcAlgorithm::Crc8ICode,
         CrcParameter::Create("CRC-8/I-CODE", 8, 0x1D, 0xFD, false, false, 0x0,
                              0x7E)},
        {CrcAlgorithm::Crc8Itu, CrcParameter::Create("CRC-8/ITU", 8, 0x7, 0x0,
                                                     false, false, 0x55, 0xA1)},
        {CrcAlgorithm::Crc8Maxim,
         CrcParameter::Create("CRC-8/MAXIM", 8, 0x31, 0x0, true, true, 0x0,
                              0xA1)},
        {CrcAlgorithm::Crc8Rohc,
         CrcParameter::Create("CRC-8/ROHC", 8, 0x7, 0xFF, true, true, 0x0,
                              0xD0)},
        {CrcAlgorithm::Crc8Wcdma,
         CrcParameter::Create("CRC-8/WCDMA", 8, 0x9B, 0x0, true, true, 0x0,
                              0x25)},

        // CRC-10
        {CrcAlgorithm::Crc10, CrcParameter::Create("CRC-10", 10, 0x233, 0x0,
                                                   false, false, 0x0, 0x199)},
        {CrcAlgorithm::Crc10Cdma2000,
         CrcParameter::Create("CRC-10/CDMA2000", 10, 0x3D9, 0x3FF, false, false,
                              0x0, 0x233)},

        // CRC-11
        {CrcAlgorithm::Crc11, CrcParameter::Create("CRC-11", 11, 0x385, 0x1A,
                                                   false, false, 0x0, 0x5A3)},

        // CRC-12
        {CrcAlgorithm::Crc123Gpp,
         CrcParameter::Create("CRC-12/3GPP", 12, 0x80F, 0x0, false, true, 0x0,
                              0xDAF)},
        {CrcAlgorithm::Crc12Cdma2000,
         CrcParameter::Create("CRC-12/CDMA2000", 12, 0xF13, 0xFFF, false, false,
                              0x0, 0xD4D)},
        {CrcAlgorithm::Crc12Dect,
         CrcParameter::Create("CRC-12/DECT", 12, 0x80F, 0x0, false, false, 0x0,
                              0xF5B)},

        // CRC-13
        {CrcAlgorithm::Crc13Bbc,
         CrcParameter::Create("CRC-13/BBC", 13, 0x1CF5, 0x0, false, false, 0x0,
                              0x4FA)},

        // CRC-14
        {CrcAlgorithm::Crc14Darc,
         CrcParameter::Create("CRC-14/DARC", 14, 0x805, 0x0, true, true, 0x0,
                              0x82D)},

        // CRC-15
        {CrcAlgorithm::Crc15, CrcParameter::Create("CRC-15", 15, 0x4599, 0x0,
                                                   false, false, 0x0, 0x59E)},
        {CrcAlgorithm::Crc15Mpt1327,
         CrcParameter::Create("CRC-15/MPT1327", 15, 0x6815, 0x0, false, false,
                              0x1, 0x2566)},

        // CRC-16
        {CrcAlgorithm::Crc16CcittFalse,
         CrcParameter::Create("CRC-16/CCITT-FALSE", 16, 0x1021, 0xFFFF, false,
                              false, 0x0, 0x29B1)},
        {CrcAlgorithm::Crc16Arc,
         CrcParameter::Create("CRC-16/ARC", 16, 0x8005, 0x0, true, true, 0x0,
                              0xBB3D)},
        {CrcAlgorithm::Crc16AugCcitt,
         CrcParameter::Create("CRC-16/AUG-CCITT", 16, 0x1021, 0x1D0F, false,
                              false, 0x0, 0xE5CC)},
        {CrcAlgorithm::Crc16Buypass,
         CrcParameter::Create("CRC-16/BUYPASS", 16, 0x8005, 0x0, false, false,
                              0x0, 0xFEE8)},
        {CrcAlgorithm::Crc16Cdma2000,
         CrcParameter::Create("CRC-16/CDMA2000", 16, 0xC867, 0xFFFF, false,
                              false, 0x0, 0x4C06)},
        {CrcAlgorithm::Crc16Dds110,
         CrcParameter::Create("CRC-16/DDS-110", 16, 0x8005, 0x800D, false,
                              false, 0x0, 0x9ECF)},
        {CrcAlgorithm::Crc16DectR,
         CrcParameter::Create("CRC-16/DECT-R", 16, 0x589, 0x0, false, false,
                              0x1, 0x7E)},
        {CrcAlgorithm::Crc16DectX,
         CrcParameter::Create("CRC-16/DECT-X", 16, 0x589, 0x0, false, false,
                              0x0, 0x7F)},
        {CrcAlgorithm::Crc16Dnp,
         CrcParameter::Create("CRC-16/DNP", 16, 0x3D65, 0x0, true, true, 0xFFFF,
                              0xEA82)},
        {CrcAlgorithm::Crc16En13757,
         CrcParameter::Create("CRC-16/EN-13757", 16, 0x3D65, 0x0, false, false,
                              0xFFFF, 0xC2B7)},
        {CrcAlgorithm::Crc16Genibus,
         CrcParameter::Create("CRC-16/GENIBUS", 16, 0x1021, 0xFFFF, false,
                              false, 0xFFFF, 0xD64E)},
        {CrcAlgorithm::Crc16Ibm,
         CrcParameter::Create("CRC-16/IBM", 16, 0x8005, 0x0, true, true, 0x0,
                              0xD64E)},
        {CrcAlgorithm::Crc16Maxim,
         CrcParameter::Create("CRC-16/MAXIM", 16, 0x8005, 0x0, true, true,
                              0xFFFF, 0x44C2)},
        {CrcAlgorithm::Crc16Mcrf4Xx,
         CrcParameter::Create("CRC-16/MCRF4XX", 16, 0x1021, 0xFFFF, true, true,
                              0x0, 0x6F91)},
        {CrcAlgorithm::Crc16Riello,
         CrcParameter::Create("CRC-16/RIELLO", 16, 0x1021, 0xB2AA, true, true,
                              0x0, 0x63D0)},
        {CrcAlgorithm::Crc16T10Dif,
         CrcParameter::Create("CRC-16/T10-DIF", 16, 0x8BB7, 0x0, false, false,
                              0x0, 0xD0DB)},
        {CrcAlgorithm::Crc16Teledisk,
         CrcParameter::Create("CRC-16/TELEDISK", 16, 0xA097, 0x0, false, false,
                              0x0, 0xFB3)},
        {CrcAlgorithm::Crc16Tms37157,
         CrcParameter::Create("CRC-16/TMS37157", 16, 0x1021, 0x89EC, true, true,
                              0x0, 0x26B1)},
        {CrcAlgorithm::Crc16Usb,
         CrcParameter::Create("CRC-16/USB", 16, 0x8005, 0xFFFF, true, true,
                              0xFFFF, 0xB4C8)},
        {CrcAlgorithm::CrcA, CrcParameter::Create("CRC-A", 16, 0x1021, 0xC6C6,
                                                  true, true, 0x0, 0xBF05)},
        {CrcAlgorithm::Crc16Kermit,
         CrcParameter::Create("CRC-16/KERMIT", 16, 0x1021, 0x0, true, true, 0x0,
                              0x2189)},
        {CrcAlgorithm::Crc16Modbus,
         CrcParameter::Create("CRC-16/MODBUS", 16, 0x8005, 0xFFFF, true, true,
                              0x0, 0x4B37)},
        {CrcAlgorithm::Crc16X25,
         CrcParameter::Create("CRC-16/X-25", 16, 0x1021, 0xFFFF, true, true,
                              0xFFFF, 0x906E)},
        {CrcAlgorithm::Crc16Xmodem,
         CrcParameter::Create("CRC-16/XMODEM", 16, 0x1021, 0x0, false, false,
                              0x0, 0x31C3)},

        // CRC-24
        {CrcAlgorithm::Crc24,
         CrcParameter::Create("CRC-24", 24, 0x864CFB, 0xB704CE, false, false,
                              0x0, 0x21CF02)},
        {CrcAlgorithm::Crc24FlexrayA,
         CrcParameter::Create("CRC-24/FLEXRAY-A", 24, 0x5D6DCB, 0xFEDCBA, false,
                              false, 0x0, 0x7979BD)},
        {CrcAlgorithm::Crc24FlexrayB,
         CrcParameter::Create("CRC-24/FLEXRAY-B", 24, 0x5D6DCB, 0xABCDEF, false,
                              false, 0x0, 0x1F23B8)},

        // CRC-31
        {CrcAlgorithm::Crc31Philips,
         CrcParameter::Create("CRC-31/PHILIPS", 31, 0x4C11DB7, 0x7FFFFFFF,
                              false, false, 0x7FFFFFFF, 0xCE9E46C)},

        // CRC-32
        {CrcAlgorithm::Crc32,
         CrcParameter::Create("CRC-32", 32, 0x04C11DB7, 0xFFFFFFFF, true, true,
                              0xFFFFFFFF, 0xCBF43926)},
        {CrcAlgorithm::Crc32Bzip2,
         CrcParameter::Create("CRC-32/BZIP2", 32, 0x04C11DB7, 0xFFFFFFFF, false,
                              false, 0xFFFFFFFF, 0xFC891918)},
        {CrcAlgorithm::Crc32C,
         CrcParameter::Create("CRC-32C", 32, 0x1EDC6F41, 0xFFFFFFFF, true, true,
                              0xFFFFFFFF, 0xE3069283)},
        {CrcAlgorithm::Crc32D,
         CrcParameter::Create("CRC-32D", 32, 0xA833982B, 0xFFFFFFFF, true, true,
                              0xFFFFFFFF, 0x87315576)},
        {CrcAlgorithm::Crc32Jamcrc,
         CrcParameter::Create("CRC-32/JAMCRC", 32, 0x04C11DB7, 0xFFFFFFFF, true,
                              true, 0x00000000, 0x340BC6D9)},
        {CrcAlgorithm::Crc32Mpeg2,
         CrcParameter::Create("CRC-32/MPEG-2", 32, 0x04C11DB7, 0xFFFFFFFF,
                              false, false, 0x00000000, 0x0376E6E7)},
        {CrcAlgorithm::Crc32Posix,
         CrcParameter::Create("CRC-32/POSIX", 32, 0x04C11DB7, 0x00000000, false,
                              false, 0xFFFFFFFF, 0x765E7680)},
        {CrcAlgorithm::Crc32Q,
         CrcParameter::Create("CRC-32Q", 32, 0x814141AB, 0x00000000, false,
                              false, 0x00000000, 0x3010BF7F)},
        {CrcAlgorithm::Crc32Xfer,
         CrcParameter::Create("CRC-32/XFER", 32, 0x000000AF, 0x00000000, false,
                              false, 0x00000000, 0xBD0BE338)},

        // CRC-40
        {CrcAlgorithm::Crc40Gsm,
         CrcParameter::Create("CRC-40/GSM", 40, 0x4820009, 0x0, false, false,
                              0xFFFFFFFFFF, 0xD4164FC646)},

        // CRC-64
        {CrcAlgorithm::Crc64,
         CrcParameter::Create("CRC-64", 64, 0x42F0E1EBA9EA3693, 0x00000000,
                              false, false, 0x00000000, 0x6C40DF5F0B497347)},
        {CrcAlgorithm::Crc64We,
         CrcParameter::Create("CRC-64/WE", 64, 0x42F0E1EBA9EA3693,
                              0xFFFFFFFFFFFFFFFF, false, false,
                              0xFFFFFFFFFFFFFFFF, 0x62EC59E3F1A4F00A)},
        {CrcAlgorithm::Crc64Xz,
         CrcParameter::Create("CRC-64/XZ", 64, 0x42F0E1EBA9EA3693,
                              0xFFFFFFFFFFFFFFFF, true, true,
                              0xFFFFFFFFFFFFFFFF, 0x995DC9BBDF1939FA)}};
  });
  auto it = algos.find(algo);
  if (it == algos.end()) {
    return nullptr;
  }
  return it->second;
}

CrcCalculator::CrcCalculator(const CrcParameterPtr& param) : m_param(param) {
  m_currentValue = m_param->initialValue() & m_param->mask();
}

CrcCalculator::CrcCalculator(uint8_t width, uint64_t polynomial,
                             uint64_t initValue, bool reflectIn,
                             bool reflectOut, uint64_t xorOut)
    : m_param(std::make_shared<CrcParameter>("", width, polynomial, initValue,
                                             reflectIn, reflectOut, xorOut)) {
  m_currentValue = m_param->initialValue() & m_param->mask();
}

void CrcCalculator::reset() {
  m_currentValue = m_param->initialValue() & m_param->mask();
}

CrcParameterPtr CrcCalculator::parameter() const { return m_param; }

uint64_t CrcCalculator::calc(const void* data, uint64_t length) {
  this->reset();
  this->update(data, length);
  uint64_t crc = this->finalize();
  this->reset();
  return crc;
}

uint64_t CrcCalculator::calc(const void* data, uint64_t length,
                             uint64_t prevResult) {
  prevResult &= m_param->mask();
  prevResult ^= m_param->xorOutput();
  if (m_param->reflectOutput()) {
    prevResult = ReflectBits(prevResult, m_param->width());
  }

  m_currentValue = prevResult;
  this->update(data, length);

  uint64_t crc = this->finalize();

  this->reset();
  return crc;
}

void CrcCalculator::update(const void* data, uint64_t length) {
  uint64_t crc = m_currentValue;
  const uint8_t* p = static_cast<const uint8_t*>(data);

  int shift = m_param->width() - 8;
  for (uint64_t i = 0; i < length; i++) {
    auto byte = p[i];
    if (m_param->reflectInput()) byte = ReflectBitOrderTable[byte];

    uint64_t index = (crc >> shift) ^ (uint64_t)byte;
    crc = (crc << 8) ^ m_param->eflectBit(index & 0xFF);
  }

  crc &= m_param->mask();
  m_currentValue = crc;
}

uint64_t CrcCalculator::finalize() {
  auto crc = m_currentValue;
  if (m_param->reflectOutput()) {
    crc = ReflectBits(crc, m_param->width());
  }
  crc ^= m_param->xorOutput();
  crc &= m_param->mask();
  return crc;
}

NS_FF_END
