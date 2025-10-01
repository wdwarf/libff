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

class Parameter {
 public:
  Parameter(uint8_t width, uint64_t polynomial, uint64_t initValue,
            bool reflectIn, bool reflectOut, uint64_t xorOut);

  uint64_t polynomial() const;
  uint64_t initialValue() const;
  bool reflectInput() const;
  bool reflectOutput() const;
  uint64_t xorOutput() const;
  uint8_t width() const;
  uint64_t mask() const;
  uint64_t highBit() const;

  uint64_t eflectBit(uint8_t v) const;

 private:
  uint8_t m_width;
  uint64_t m_polynomial;
  uint64_t m_initialValue;
  bool m_reflectInput;
  bool m_reflectOutput;
  uint64_t m_xorOutput;
  uint64_t m_mask;
  uint64_t m_highBit;
  std::vector<uint64_t> m_eflectBitOrderTable = std::vector<uint64_t>(256);
  void updateTable();
};

using ParameterPtr = std::shared_ptr<Parameter>;

class Calculator {
 public:
  Calculator(const ParameterPtr& param);
  Calculator(uint8_t width, uint64_t polynomial, uint64_t initValue,
             bool reflectIn, bool reflectOut, uint64_t xorOut);

  uint64_t calc(const void* data, uint64_t length);
  uint64_t calc(const void* data, uint64_t length, uint64_t prevResult);
  void update(const void* data, uint64_t length);
  uint64_t finalize();
  void reset();
  ParameterPtr parameter() const;

 private:
  ParameterPtr m_param;
  uint64_t m_currentValue = 0;
};

NS_FF_END

#endif /* FF_CRC_H_ */
