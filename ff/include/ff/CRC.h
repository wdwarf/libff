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
#include <map>
#include <memory>
#include <string>
#include <vector>

NS_FF_BEG

enum class CrcAlgorithm {
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
  Crc16Ibm,
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

class CrcParameter;
using CrcParameterPtr = std::shared_ptr<CrcParameter>;

class CrcParameter {
 public:
  CrcParameter(const std::string& name, uint8_t width, uint64_t polynomial,
               uint64_t initValue, bool reflectIn, bool reflectOut,
               uint64_t xorOut, uint64_t check = 0);

  static CrcParameterPtr Create(const std::string& name, uint8_t width,
                                uint64_t polynomial, uint64_t initValue,
                                bool reflectIn, bool reflectOut,
                                uint64_t xorOut, uint64_t check = 0);
  static CrcParameterPtr Create(CrcAlgorithm algo);

  std::string name() const;
  uint64_t polynomial() const;
  uint64_t initialValue() const;
  bool reflectInput() const;
  bool reflectOutput() const;
  uint64_t xorOutput() const;
  uint8_t width() const;
  uint64_t check() const;
  uint64_t mask() const;
  uint64_t highBit() const;

  uint64_t eflectBit(uint8_t v) const;

 private:
  std::string m_name;
  uint8_t m_width;
  uint64_t m_polynomial;
  uint64_t m_initialValue;
  bool m_reflectInput;
  bool m_reflectOutput;
  uint64_t m_xorOutput;
  uint64_t m_check;
  uint64_t m_mask;
  uint64_t m_highBit;
  std::vector<uint64_t> m_eflectBitOrderTable = std::vector<uint64_t>(256);
  void updateTable();
};

class CrcCalculator {
 public:
  CrcCalculator(const CrcParameterPtr& param);
  CrcCalculator(uint8_t width, uint64_t polynomial, uint64_t initValue,
                bool reflectIn, bool reflectOut, uint64_t xorOut);

  uint64_t calc(const void* data, uint64_t length);
  uint64_t calc(const void* data, uint64_t length, uint64_t prevResult);
  void update(const void* data, uint64_t length);
  uint64_t finalize();
  void reset();
  CrcParameterPtr parameter() const;

 private:
  CrcParameterPtr m_param;
  uint64_t m_currentValue = 0;
};

NS_FF_END

#endif /* FF_CRC_H_ */
