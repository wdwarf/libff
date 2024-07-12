/**
 * @file Payload.h
 * @author liyawu
 * @date 2024-07-12 10:24:30
 * @description
 */

#ifndef _FF_PAYLOAD_H
#define _FF_PAYLOAD_H

#include <ff/ff_config.h>

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

NS_FF_BEG

enum class PayloadDataType : uint8_t {
  Unknown = 0,
  Boolean,
  Sint,
  Uint,
  Float,
  String,
  CArray
};

std::ostream& operator<<(std::ostream& o, const PayloadDataType& pt);

class LIBFF_API PayloadReader {
 public:
  PayloadReader(const uint8_t* payload, uint32_t len);

  template <typename T>
  const T* read();

  template <typename T>
  bool read(T& val);
  bool read(void* buf, uint32_t& len);

  template <typename T, size_t L>
  bool read(T (&buf)[L]) {
    uint32_t len = sizeof(buf);
    return read(buf, len);
  }

  PayloadDataType nextDataType() const;
  bool nextDataType(PayloadDataType& type, uint32_t& len) const;

  void reset();

  std::string print();

 private:
  const uint8_t* m_payload;
  uint32_t m_payloadLen;
  uint32_t m_rpos = 0;
  static void ParseFlag(uint8_t flag, PayloadDataType& type, uint8_t& len);
};

class LIBFF_API PayloadWriter {
 public:
  PayloadWriter(std::vector<uint8_t>& payload);

  template <typename T>
  bool write(const T& val);

  template <typename T>
  PayloadWriter& operator<<(const T& val) {
    this->write(val);
    return *this;
  }

  bool write(const char* str);
  bool write(const void* buf, uint32_t len);
  PayloadWriter& operator<<(const char* str) {
    this->write(str);
    return *this;
  }

  template <typename T, size_t L>
  bool write(const T (&buf)[L]) {
    uint32_t len = sizeof(buf);
    return write(buf, len);
  }

  template <typename T, size_t L>
  PayloadWriter& operator<<(const T (&buf)[L]) {
    uint32_t len = sizeof(buf);
    return write(buf, len);
    return *this;
  }

 private:
  std::vector<uint8_t>& m_payload;
  static uint8_t MakeFlag(PayloadDataType type, uint8_t len);
};

class LIBFF_API Payload {
 public:
  Payload() = default;
  Payload(const uint8_t* payload, uint32_t len);
  Payload(const std::vector<uint8_t>& payload);

  void reset();
  const uint8_t* data() const;
  uint32_t length() const;
  void append(const Payload& payload);
  PayloadReader reader() const;
  PayloadWriter writer();

  std::string print();

 private:
  std::vector<uint8_t> m_payload;
};

NS_FF_END

#endif  // _FF_PAYLOAD_H
