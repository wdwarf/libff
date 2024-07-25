/**
 * @file CycleBuffer.h
 * @author liyawu
 * @date 2024-07-23 11:21:36
 * @description
 */

#ifndef _CYCLEBUFFER_H
#define _CYCLEBUFFER_H

#include <ff/ff_config.h>

#include <vector>
#include <algorithm>

NS_FF_BEG

#ifndef min
#define min std::min
#endif

template <typename T>
class CycleBuffer {
 public:
  using SizeType = uint32_t;
  CycleBuffer(SizeType capacity) {
    m_capacity = capacity;
    m_size = 0;
    m_pos = 0;
    m_buf = new T[capacity];
  }

  ~CycleBuffer() {
    if (m_buf) delete[] m_buf;
  }

  SizeType size() const { return m_size; }

  bool isFull() const { return m_size == m_capacity; }

  bool isEmpty() const { return m_size <= 0; }

  bool put(T o) {
    if (isFull()) return false;
    auto tailPos = tail();
    m_buf[tailPos] = o;
    ++m_size;
    return true;
  }

  bool get(T& o) {
    if (isEmpty()) return false;
    o = m_buf[m_pos];

    m_pos = (m_pos + 1) % m_capacity;
    --m_size;
    return true;
  }

  SizeType read(T* o, SizeType size) {
    if (isEmpty()) return 0;
    auto size2Read = min(size, m_size);

    for (uint32_t i = 0; i < size2Read; ++i) {
      auto tmpPos = (m_pos + i) % m_capacity;
      o[i] = m_buf[tmpPos];
    }

    m_pos = (m_pos + size2Read) % m_capacity;
    m_size -= size2Read;

    return size2Read;
  }

  SizeType pos() const { return m_pos; }

 private:
  SizeType m_capacity = 0;
  SizeType m_size = 0;
  SizeType m_pos = 0;
  T* m_buf = nullptr;
  SizeType tail() const { return (m_pos + m_size) % m_capacity; }
};

using CharCycleBuffer = CycleBuffer<char>;
using ByteCycleBuffer = CycleBuffer<uint8_t>;

NS_FF_END

#endif  // _CYCLEBUFFER_H
