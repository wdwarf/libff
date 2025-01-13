/**
 * @file HexNumberFormater.cpp
 * @author liyawu
 * @date 2025-01-13 14:54:11
 * @description
 */
#include <ff/HexNumberFormater.h>
#include <ff/String.h>

#include <iomanip>
#include <sstream>

NS_FF_BEG

namespace {

std::string Format(uint64_t value, uint8_t width, bool fillZero, bool usePrefix,
                   bool uppercase) {
  std::stringstream str;
  if (usePrefix) str << "0x";
  str << std::hex;
  if (uppercase) str << std::uppercase;
  if (fillZero) str << std::setw(width) << std::setfill('0');
  str << value;
  return str.str();
}

}  // namespace

HexNumberFormater::HexNumberFormater() {}

HexNumberFormater::HexNumberFormater(bool fillZero, bool usePrefix,
                                     bool uppercase)
    : m_fillZero(fillZero), m_usePrefix(usePrefix), m_uppercase(uppercase) {}

HexNumberFormater::~HexNumberFormater() {}

void HexNumberFormater::fillZero(bool enabled) { m_fillZero = enabled; }

void HexNumberFormater::usePrefix(bool enabled) { m_usePrefix = enabled; }

void HexNumberFormater::uppercase(bool enabled) { m_uppercase = enabled; }

std::string HexNumberFormater::format(uint8_t value) const {
  return Format(value, 2, m_fillZero, m_usePrefix, m_uppercase);
}

std::string HexNumberFormater::format(uint16_t value) const {
  return Format(value, 4, m_fillZero, m_usePrefix, m_uppercase);
}

std::string HexNumberFormater::format(uint32_t value) const {
  return Format(value, 8, m_fillZero, m_usePrefix, m_uppercase);
}

std::string HexNumberFormater::format(uint64_t value) const {
  return Format(value, 16, m_fillZero, m_usePrefix, m_uppercase);
}

uint64_t HexNumberFormater::parse(const std::string& hexStr) const {
  auto str = ToLowerCopy(hexStr);
  Trim(str);
  if (str.find("0x") != std::string::npos) str = str.substr(2);
  std::stringstream ss;
  ss << str;
  uint64_t value = 0;
  ss >> std::hex >> value;
  return value;
}

NS_FF_END
