/**
 * @file HexNumberFormater.h
 * @author liyawu
 * @date 2025-01-13 14:53:41
 * @description
 */

#ifndef _HEXNUMBERFORMATER_H
#define _HEXNUMBERFORMATER_H

#include <ff/ff_config.h>

#include <cstdint>
#include <string>

NS_FF_BEG

class HexNumberFormater {
 public:
  HexNumberFormater();
  HexNumberFormater(bool fillZero, bool usePrefix, bool uppercase);
  ~HexNumberFormater();

  void fillZero(bool enabled);
  void usePrefix(bool enabled);
  void uppercase(bool enabled);

  std::string format(uint8_t value) const;
  std::string format(uint16_t value) const;
  std::string format(uint32_t value) const;
  std::string format(uint64_t value) const;
  uint64_t parse(const std::string& hexStr) const;

 private:
  bool m_fillZero = false;
  bool m_usePrefix = false;
  bool m_uppercase = false;
};

NS_FF_END

#endif  // _HEXNUMBERFORMATER_H
