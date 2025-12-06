/*
 * CRC.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: ducky
 */

#include <ff/Buffer.h>
#include <ff/CRC.h>

#include <iostream>

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

CrcParameter::CrcParameter(uint8_t width, uint64_t polynomial,
                           uint64_t initValue, bool reflectIn, bool reflectOut,
                           uint64_t xorOut)
    : m_width(width),
      m_polynomial(polynomial),
      m_initialValue(initValue),
      m_reflectInput(reflectIn),
      m_reflectOutput(reflectOut),
      m_xorOutput(xorOut) {
  m_mask = (1ull << m_width) - 1;
  m_highBit = 1ull << (m_width - 1);
  this->updateTable();
}

CrcParameterPtr CrcParameter::Create(uint8_t width, uint64_t polynomial,
                                     uint64_t initValue, bool reflectIn,
                                     bool reflectOut, uint64_t xorOut) {
  return std::make_shared<CrcParameter>(width, polynomial, initValue, reflectIn,
                                        reflectOut, xorOut);
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

uint64_t CrcParameter::polynomial() const { return m_polynomial; }

uint64_t CrcParameter::initialValue() const { return m_initialValue; }

bool CrcParameter::reflectInput() const { return m_reflectInput; }

bool CrcParameter::reflectOutput() const { return m_reflectOutput; }

uint64_t CrcParameter::xorOutput() const { return m_xorOutput; }

uint8_t CrcParameter::width() const { return m_width; }

uint64_t CrcParameter::mask() const { return m_mask; }

uint64_t CrcParameter::highBit() const { return m_highBit; }

uint64_t CrcParameter::eflectBit(uint8_t v) const {
  return m_eflectBitOrderTable[v];
}

CrcCalculator::CrcCalculator(const CrcParameterPtr& param) : m_param(param) {
  m_currentValue = m_param->initialValue() & m_param->mask();
}

CrcCalculator::CrcCalculator(uint8_t width, uint64_t polynomial,
                             uint64_t initValue, bool reflectIn,
                             bool reflectOut, uint64_t xorOut)
    : m_param(std::make_shared<CrcParameter>(width, polynomial, initValue,
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
