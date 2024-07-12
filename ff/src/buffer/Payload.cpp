/**
 * @file Payload.cpp
 * @author liyawu
 * @date 2024-07-12 10:25:53
 * @description
 */
#include <ff/Payload.h>

#include <sstream>

using namespace std;

NS_FF_BEG

std::ostream &operator<<(std::ostream &o, const PayloadDataType &pt) {
  o << (PayloadDataType::Unknown == pt   ? "PayloadDataType::Unknown"
        : PayloadDataType::Boolean == pt ? "PayloadDataType::Boolean"
        : PayloadDataType::Sint == pt    ? "PayloadDataType::Sint"
        : PayloadDataType::Uint == pt    ? "PayloadDataType::Uint"
        : PayloadDataType::Float == pt   ? "PayloadDataType::Float"
        : PayloadDataType::String == pt  ? "PayloadDataType::String"
        : PayloadDataType::CArray == pt  ? "PayloadDataType::CArray"
                                         : "");
  return o;
}

// ===========================================
// ===========================================

PayloadReader::PayloadReader(const uint8_t *payload, uint32_t len)
    : m_payload(payload), m_payloadLen(len) {}

void PayloadReader::reset() { m_rpos = 0; }

PayloadDataType PayloadReader::nextDataType() const {
  if (m_rpos >= m_payloadLen) return PayloadDataType::Unknown;

  return PayloadDataType(m_payload[m_rpos] >> 4);
}

bool PayloadReader::nextDataType(PayloadDataType &type, uint32_t &len) const {
  if (m_rpos >= m_payloadLen) {
    len = 0;
    type = PayloadDataType::Unknown;
    return false;
  }

  type = PayloadDataType(m_payload[m_rpos] >> 4);
  len = m_payload[m_rpos] & 0x0F;
  if (0 == len) {
    memcpy(&len, &m_payload[m_rpos + 1], sizeof(len));
  }
  return true;
}

void PayloadReader::ParseFlag(uint8_t flag, PayloadDataType &type,
                              uint8_t &len) {
  type = PayloadDataType(flag >> 4);
  len = flag & 0x0F;
}

#define UINT_PTR_R_FUNC_DEF(VT, PT)                           \
  template <>                                                 \
  const VT *PayloadReader::read<VT>() {                       \
    if ((m_rpos + sizeof(VT)) > m_payloadLen) return nullptr; \
    int8_t flag = m_payload[m_rpos];                          \
    PayloadDataType type = PayloadDataType::Unknown;          \
    uint8_t len = 0;                                          \
    ParseFlag(flag, type, len);                               \
    if (PT != type || sizeof(VT) != len) return nullptr;      \
                                                              \
    ++m_rpos;                                                 \
    const VT *val = (const VT *)&m_payload[m_rpos];           \
    m_rpos += len;                                            \
    return val;                                               \
  }

UINT_PTR_R_FUNC_DEF(int8_t, PayloadDataType::Sint)
UINT_PTR_R_FUNC_DEF(int16_t, PayloadDataType::Sint)
UINT_PTR_R_FUNC_DEF(int32_t, PayloadDataType::Sint)
UINT_PTR_R_FUNC_DEF(int64_t, PayloadDataType::Sint)
UINT_PTR_R_FUNC_DEF(uint8_t, PayloadDataType::Uint)
UINT_PTR_R_FUNC_DEF(uint16_t, PayloadDataType::Uint)
UINT_PTR_R_FUNC_DEF(uint32_t, PayloadDataType::Uint)
UINT_PTR_R_FUNC_DEF(uint64_t, PayloadDataType::Uint)

template <>
bool PayloadReader::read(bool &val) {
  if (m_rpos >= m_payloadLen) return false;
  int8_t flag = m_payload[m_rpos];
  PayloadDataType type = PayloadDataType::Unknown;
  uint8_t len = 0;
  ParseFlag(flag, type, len);
  if (PayloadDataType::Boolean != type) return false;
  ++m_rpos;
  val = (0 != len);
  return true;
}

template <>
bool PayloadReader::read(float &val) {
  if ((m_rpos + sizeof(val)) > m_payloadLen) return false;
  int8_t flag = m_payload[m_rpos];
  PayloadDataType type = PayloadDataType::Unknown;
  uint8_t len = 0;
  ParseFlag(flag, type, len);
  if (PayloadDataType::Float != type || sizeof(val) != len) return false;

  ++m_rpos;
  memcpy(&val, &m_payload[m_rpos], len);
  m_rpos += len;
  return true;
}

template <>
bool PayloadReader::read(double &val) {
  if ((m_rpos + sizeof(val)) > m_payloadLen) return false;
  int8_t flag = m_payload[m_rpos];
  PayloadDataType type = PayloadDataType::Unknown;
  uint8_t len = 0;
  ParseFlag(flag, type, len);
  if (PayloadDataType::Float != type || sizeof(val) != len) return false;

  ++m_rpos;
  memcpy(&val, &m_payload[m_rpos], len);
  m_rpos += len;
  return true;
}

template <>
bool PayloadReader::read(std::string &val) {
  if (m_rpos >= m_payloadLen) return false;
  int8_t flag = m_payload[m_rpos];
  PayloadDataType type = PayloadDataType::Unknown;
  uint8_t len = 0;
  ParseFlag(flag, type, len);
  if (PayloadDataType::String != type) return false;
  ++m_rpos;

  uint32_t strLen = len;
  if (0 == strLen) {
    memcpy(&strLen, &m_payload[m_rpos], sizeof(uint32_t));
    m_rpos += sizeof(uint32_t);
  }

  val = "";
  for (uint32_t i = 0; i < strLen - 1; ++i) {
    val.push_back(m_payload[m_rpos + i]);
  }

  m_rpos += strLen;
  return true;
}

bool PayloadReader::read(void *buf, uint32_t &bufLen) {
  if (m_rpos >= m_payloadLen) return false;

  PayloadDataType type = PayloadDataType::Unknown;
  uint32_t len = 0;
  nextDataType(type, len);
  if (PayloadDataType::CArray != type || len > bufLen) return false;
  ++m_rpos;

  if (len > 0x0F) {
    m_rpos += sizeof(uint32_t);
  }

  memcpy(buf, &m_payload[m_rpos], len);

  bufLen = len;
  m_rpos += len;
  return true;
}

#define INT_READ_FUNC_DEF(VT, PT)                            \
  template <>                                                \
  bool PayloadReader::read(VT &val) {                        \
    if ((m_rpos + sizeof(val)) > m_payloadLen) return false; \
    int8_t flag = m_payload[m_rpos];                         \
    PayloadDataType type = PayloadDataType::Unknown;         \
    uint8_t len = 0;                                         \
    ParseFlag(flag, type, len);                              \
    if (PT != type || sizeof(val) != len) return false;      \
    ++m_rpos;                                                \
    memcpy(&val, &m_payload[m_rpos], len);                   \
    m_rpos += len;                                           \
    return true;                                             \
  }

INT_READ_FUNC_DEF(int8_t, PayloadDataType::Sint)
INT_READ_FUNC_DEF(int16_t, PayloadDataType::Sint)
INT_READ_FUNC_DEF(int32_t, PayloadDataType::Sint)
INT_READ_FUNC_DEF(int64_t, PayloadDataType::Sint)
INT_READ_FUNC_DEF(uint8_t, PayloadDataType::Uint)
INT_READ_FUNC_DEF(uint16_t, PayloadDataType::Uint)
INT_READ_FUNC_DEF(uint32_t, PayloadDataType::Uint)
INT_READ_FUNC_DEF(uint64_t, PayloadDataType::Uint)

std::string PayloadReader::print() {
  PayloadReader reader(this->m_payload, this->m_payloadLen);

  stringstream str;
  PayloadDataType type = PayloadDataType::Unknown;
  uint32_t len = 0;
  while (reader.nextDataType(type, len)) {
    str << type;
    switch (type) {
      case PayloadDataType::Boolean: {
        bool b = false;
        reader.read(b);
        str << ": " << (b ? "true" : "false");
      } break;
      case PayloadDataType::Sint: {
        switch (len) {
          case 1: {
            int8_t v = 0;
            reader.read(v);
            str << (len * 8) << ": " << int32_t(v);
          } break;
          case 2: {
            int16_t v = 0;
            reader.read(v);
            str << (len * 8) << ": " << v;
          } break;
          case 4: {
            int32_t v = 0;
            reader.read(v);
            str << (len * 8) << ": " << v;
          } break;
          case 8: {
            int64_t v = 0;
            reader.read(v);
            str << (len * 8) << ": " << v;
          } break;
        }
      } break;
      case PayloadDataType::Uint: {
        switch (len) {
          case 1: {
            uint8_t v = 0;
            reader.read(v);
            str << (len * 8) << ": " << int32_t(v);
          } break;
          case 2: {
            uint16_t v = 0;
            reader.read(v);
            str << (len * 8) << ": " << v;
          } break;
          case 4: {
            uint32_t v = 0;
            reader.read(v);
            str << (len * 8) << ": " << v;
          } break;
          case 8: {
            uint64_t v = 0;
            reader.read(v);
            str << (len * 8) << ": " << v;
          } break;
        }
      } break;
      case PayloadDataType::Float: {
        switch (len) {
          case 4: {
            float v = 0;
            reader.read(v);
            str << (len * 8) << ": " << v;
          } break;
          case 8: {
            double v = 0;
            reader.read(v);
            str << (len * 8) << ": " << v;
          } break;
        }
      } break;
      case PayloadDataType::String: {
        string s;
        reader.read(s);
        str << ": " << s;
      } break;
      case PayloadDataType::CArray: {
        str << "[" << len << "]: ";
        std::vector<uint8_t> buf(len);
        reader.read(&buf[0], len);
        for (auto c : buf) {
          str << hex << setw(2) << setfill('0') << (uint16_t)c << " ";
        }
        str << dec;
      } break;
      default:
        return "";
    }

    str << endl;
  }

  return str.str();
}

// ===========================================
// ===========================================

PayloadWriter::PayloadWriter(std::vector<uint8_t> &payload)
    : m_payload(payload) {}

uint8_t PayloadWriter::MakeFlag(PayloadDataType type, uint8_t len) {
  return (uint8_t(type) << 4) | (len & 0x0F);
}

template <>
bool PayloadWriter::write(const bool &val) {
  m_payload.emplace_back(MakeFlag(PayloadDataType::Boolean, val ? 1 : 0));
  return true;
}

template <>
bool PayloadWriter::write(const float &val) {
  auto valSize = sizeof(val);
  m_payload.emplace_back(MakeFlag(PayloadDataType::Float, valSize));
  const uint8_t *p = (const uint8_t *)&val;
  for (uint8_t i = 0; i < valSize; ++i, ++p) m_payload.emplace_back(*p);
  return true;
}

template <>
bool PayloadWriter::write(const double &val) {
  auto valSize = sizeof(val);
  m_payload.emplace_back(MakeFlag(PayloadDataType::Float, valSize));
  const uint8_t *p = (const uint8_t *)&val;
  for (uint8_t i = 0; i < valSize; ++i, ++p) m_payload.emplace_back(*p);
  return true;
}

template <>
bool PayloadWriter::write(const std::string &val) {
  uint32_t valSize = val.length() + 1;
  m_payload.emplace_back(
      MakeFlag(PayloadDataType::String, valSize > 0x0F ? 0 : valSize));

  if (valSize > 0x0F) {
    const uint8_t *p = (const uint8_t *)&valSize;
    for (uint8_t i = 0; i < sizeof(uint32_t); ++i, ++p)
      m_payload.emplace_back(*p);
  }

  for (auto c : val) {
    m_payload.emplace_back(c);
  }

  m_payload.emplace_back('\0');

  return true;
}

bool PayloadWriter::write(const char *str) { return write(std::string(str)); }

bool PayloadWriter::write(const void *buf, uint32_t len) {
  if (nullptr == buf || 0 == len) return false;
  m_payload.emplace_back(
      MakeFlag(PayloadDataType::CArray, len > 0x0F ? 0 : len));

  if (len > 0x0F) {
    const uint8_t *p = (const uint8_t *)&len;
    for (uint8_t i = 0; i < sizeof(uint32_t); ++i, ++p)
      m_payload.emplace_back(*p);
  }

  const uint8_t *p = (const uint8_t *)buf;
  for (uint32_t i = 0; i < len; ++i, ++p) {
    m_payload.emplace_back(*p);
  }

  return true;
}

#define INT_WR_FUNC_DEF(VT, PT)                                            \
  template <>                                                              \
  bool PayloadWriter::write(const VT &val) {                               \
    auto valSize = sizeof(val);                                            \
    m_payload.emplace_back(MakeFlag(PT, valSize));                         \
    const uint8_t *p = (const uint8_t *)&val;                              \
    for (uint8_t i = 0; i < valSize; ++i, ++p) m_payload.emplace_back(*p); \
    return true;                                                           \
  }

INT_WR_FUNC_DEF(int8_t, PayloadDataType::Sint)
INT_WR_FUNC_DEF(int16_t, PayloadDataType::Sint)
INT_WR_FUNC_DEF(int32_t, PayloadDataType::Sint)
INT_WR_FUNC_DEF(int64_t, PayloadDataType::Sint)
INT_WR_FUNC_DEF(uint8_t, PayloadDataType::Uint)
INT_WR_FUNC_DEF(uint16_t, PayloadDataType::Uint)
INT_WR_FUNC_DEF(uint32_t, PayloadDataType::Uint)
INT_WR_FUNC_DEF(uint64_t, PayloadDataType::Uint)

// ===========================================
// ===========================================

Payload::Payload(const uint8_t *payload, uint32_t len) {
  m_payload.resize(len);
  memcpy(&m_payload[0], payload, len);
}

Payload::Payload(const std::vector<uint8_t> &payload) : m_payload(payload) {}

void Payload::reset() { m_payload.resize(0); }

const uint8_t *Payload::data() const {
  if (m_payload.empty()) return nullptr;
  return &m_payload[0];
}

uint32_t Payload::length() const { return m_payload.size(); }

void Payload::append(const Payload &payload) {
  if (payload.length() <= 0) return;
  auto oldSize = m_payload.size();
  m_payload.resize(oldSize + payload.length());
  memcpy(&m_payload[oldSize], payload.data(), payload.length());
}

PayloadReader Payload::reader() const {
  return PayloadReader(&m_payload[0], m_payload.size());
}

PayloadWriter Payload::writer() { return PayloadWriter(m_payload); }

std::string Payload::print() { return this->reader().print(); }

NS_FF_END
