/**
 * @file Snowflake.cpp
 * @auth DuckyLi
 * @date 2022-04-06 09:42:40
 * @description
 */
#include <ff/Snowflake.h>
#include <ff/Timestamp.h>

using namespace std;

NS_FF_BEG

static uint64_t StartTimeStamp = 1649226618508;

static uint64_t SequenceBit = 12;
static uint64_t WorkerBit = 5;
static uint64_t DatacenterBit = 5;

static uint64_t MaxSequenceID = -1L ^ (-1L << SequenceBit);
static uint64_t MaxMachineID = -1L ^ (-1L << WorkerBit);
static uint64_t MaxDatacenterID = -1L ^ (-1L << DatacenterBit);

static uint32_t WorkerShift = SequenceBit;
static uint32_t DatacenterShift = WorkerShift + WorkerBit;
static uint32_t TimestampShift = DatacenterShift + DatacenterBit;

static uint64_t ToNextMs(uint64_t currTemp) {
  uint64_t temp = 0;
  for (; (temp = Timestamp::Now()) <= currTemp;);
  return temp;
}

Snowflake::Snowflake(uint8_t datacenterId, uint8_t workerId)
    : m_lastTimestamp(Timestamp::Now()), m_sequence(0) {
  if (datacenterId > MaxDatacenterID || datacenterId < 0) {
    throw SnowflakeException("Invalid datacenterId");
  }
  if (workerId > MaxMachineID || workerId < 0) {
    throw SnowflakeException("Invalid workerId");
  }
  this->m_datacenterId = datacenterId;
  this->m_workerId = workerId;
}

Snowflake::Snowflake(uint64_t lastTimestamp, uint32_t lastSequence,
                     uint8_t datacenterId, uint8_t workerId)
    : m_lastTimestamp(lastTimestamp), m_sequence(lastSequence) {
  if (datacenterId > MaxDatacenterID || datacenterId < 0) {
    throw SnowflakeException("Invalid datacenterId");
  }
  if (workerId > MaxMachineID || workerId < 0) {
    throw SnowflakeException("Invalid workerId");
  }
  this->m_datacenterId = datacenterId;
  this->m_workerId = workerId;
}

Snowflake::~Snowflake() {}

SnowflakeID Snowflake::gen() {
  lock_guard<mutex> lk(this->m_mutex);

  auto currStmp = Timestamp::Now();
  if (currStmp < this->m_lastTimestamp) {
    throw SnowflakeException("Clock moved backwards.");
  }

  if (currStmp == this->m_lastTimestamp) {
    this->m_sequence = (this->m_sequence + 1) & MaxSequenceID;
    if (0 == this->m_sequence) {
      currStmp = ToNextMs(currStmp);
    }
  } else {
    this->m_sequence = 0;
  }

  this->m_lastTimestamp = currStmp;

  return (currStmp - StartTimeStamp) << TimestampShift  // Timestamp
         | this->m_datacenterId << DatacenterShift      // Datacenter
         | this->m_workerId << WorkerShift              // Worker
         | this->m_sequence;                            // sequence
}

NS_FF_END
