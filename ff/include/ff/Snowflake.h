/**
 * @file Snowflake.h
 * @auth DuckyLi
 * @date 2022-04-06 09:42:18
 * @description
 */

#ifndef _SNOWFLAKE_H_
#define _SNOWFLAKE_H_

#include <ff/Exception.h>
#include <ff/ff_config.h>

#include <mutex>

NS_FF_BEG

EXCEPTION_DEF(SnowflakeException);

using SnowflakeID = uint64_t;

class LIBFF_API Snowflake {
 private:
  uint64_t m_lastTimestamp;
  uint16_t m_datacenterId;
  uint16_t m_workerId;
  uint32_t m_sequence;
  std::mutex m_mutex;

 public:
  Snowflake(uint8_t datacenterId, uint8_t workerId);
  Snowflake(uint64_t lastTimestamp, uint32_t lastSequence, uint8_t datacenterId,
            uint8_t workerId);
  ~Snowflake();

  SnowflakeID gen();

  uint8_t workerId() const { return this->m_workerId; }
  uint8_t datacenterId() const { return this->m_datacenterId; }
  uint64_t lastTimestamp() const { return this->m_lastTimestamp; }
  uint32_t lastSequence() const { return this->m_sequence; }
};

NS_FF_END

#endif /* _SNOWFLAKE_H_ */
