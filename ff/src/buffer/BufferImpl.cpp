/*
 * BufferImpl.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include "BufferImpl.h"

#include <ff/String.h>

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

namespace {

const float BUF_INC_RATIO = 1.2f;

}

NS_FF_BEG

Buffer::BufferImpl::BufferImpl()
    : data(nullptr), size(0), capacity(0), readPos(0) {}

Buffer::BufferImpl::BufferImpl(uint32_t initSize)
    : data(nullptr), size(0), capacity(0), readPos(0) {
  this->alloc(initSize);
}

Buffer::BufferImpl::BufferImpl(const void* data, uint32_t size)
    : data(nullptr), size(0), capacity(0), readPos(0) {
  this->setData(data, size);
}

Buffer::BufferImpl::BufferImpl(const Buffer::BufferImpl& buffer)
    : data(nullptr), size(0), capacity(0), readPos(0) {
  this->setData(buffer.getData(), buffer.getSize());
}

int Buffer::BufferImpl::read(void* buf, uint32_t size) {
  uint32_t avaliableSize = this->getSize() - this->readPos;
  if (avaliableSize <= 0) return 0;

  uint32_t readSzie = avaliableSize > size ? size : avaliableSize;
  memcpy(buf, this->data + this->readPos, readSzie);
  this->readPos += readSzie;
  return readSzie;
}

void Buffer::BufferImpl::zero() {
  if (this->capacity > 0) {
    memset(this->data, 0, this->capacity);
  }
}

void Buffer::BufferImpl::resetReadPos() { this->readPos = 0; }

Buffer::BufferImpl& Buffer::BufferImpl::operator=(
    const Buffer::BufferImpl& buffer) {
  this->setData(buffer.getData(), buffer.getSize());
  return *this;
}

Buffer::BufferImpl Buffer::BufferImpl::operator+(
    const Buffer::BufferImpl& buffer) const {
  Buffer::BufferImpl newBuffer;
  newBuffer.append(*this);
  newBuffer.append(buffer);
  return newBuffer;
}

void Buffer::BufferImpl::attach(void* data, uint32_t size) {
  this->clear();
  this->capacity = size;
  this->size = size;
  this->data = (unsigned char*)data;
  this->readPos = 0;
}

void Buffer::BufferImpl::append(const void* data, uint32_t size) {
  if ((nullptr == data) || (size <= 0)) return;

  uint32_t reserveSize = this->capacity - this->size;
  if (size <= reserveSize) {
    memcpy(static_cast<unsigned char*>(this->data) + this->size, data, size);
    this->size += size;
    return;
  }

  uint32_t cap = (this->size + size) * BUF_INC_RATIO;
  auto newData = new unsigned char[cap];
  if (nullptr == newData) {
    throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
  }
  memcpy(newData, this->data, this->size);
  memcpy(newData + this->size, data, size);

  if (nullptr != this->data) delete[] this->data;
  this->data = newData;
  this->size += size;
  this->capacity = cap;
}

void Buffer::BufferImpl::append(const BufferImpl& buffer) {
  this->append(buffer.data, buffer.size);
}

void Buffer::BufferImpl::ReverseBytes(void* buf, int size) {
  if (!buf || (size <= 0)) {
    return;
  }

  auto pH = static_cast<unsigned char*>(buf);
  auto pE = pH + size - 1;
  while (pH < pE) {
    swap(*pH, *pE);
    ++pH;
    --pE;
  }
}

String Buffer::BufferImpl::ToHexString(const void* buf, int size) {
  stringstream strBuf;
  if (buf && size > 0) {
    const unsigned char* data = static_cast<const unsigned char*>(buf);

    strBuf.fill('0');
    for (int i = 0; i < size; ++i) {
      strBuf << setw(2) << hex << static_cast<int>(data[i]) << " ";
    }
  }

  return strBuf.str();
}

void Buffer::BufferImpl::reverse() {
  Buffer::ReverseBytes(this->data, this->size);

  this->resetReadPos();
}

void Buffer::BufferImpl::alloc(uint32_t size) {
  if (size <= 0) {
    return;
  }

  if (this->capacity >= size) {
    this->zero();
    this->resetReadPos();
    return;
  }

  this->clear();

  uint32_t cap = size * BUF_INC_RATIO;
  auto newData = new unsigned char[cap];
  if (nullptr == newData) {
    throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
  }

  this->size = size;
  this->capacity = cap;
  this->data = newData;
  this->zero();

  this->resetReadPos();
}

uint32_t Buffer::BufferImpl::getCapacity() const { return this->capacity; }

void Buffer::BufferImpl::setCapacity(uint32_t capacity) {
    if (this->capacity >= capacity) {
        this->capacity = capacity;
        return;
    }

    auto size = this->size > capacity ? capacity : this->size;
    auto newData = new uint8_t[capacity];
    if (nullptr == newData) {
        throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
    }

    memcpy(newData, this->data, size);
    this->clear();
    this->data = newData;
    this->size = size;
    this->capacity = capacity;
}

unsigned char& Buffer::BufferImpl::at(uint32_t index) {
  if (index >= this->size) {
    THROW_EXCEPTION(BufferException, "Index out of bound.", this->size);
  }
  return this->data[index];
}

const unsigned char& Buffer::BufferImpl::at(uint32_t index) const {
  if (index >= this->size) {
    THROW_EXCEPTION(BufferException, "Index out of bound.", this->size);
  }
  return this->data[index];
}

void Buffer::BufferImpl::setData(const void* data, uint32_t size) {
  auto oldData = this->data;
  if ((nullptr != data) && (size > 0)) {
    uint32_t cap = size * BUF_INC_RATIO;
    auto newData = new unsigned char[cap];
    if (nullptr == newData) {
      throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
    }
    this->size = size;
    this->capacity = cap;
    this->data = newData;
    memcpy(this->data, data, this->size);
  } else {
    this->size = 0;
    this->data = NULL;
    this->capacity = 0;
  }

  this->resetReadPos();

  if (oldData) delete[] oldData;
}

Buffer::BufferImpl::~BufferImpl() { this->clear(); }

void Buffer::setData(const void* data, uint32_t size) {
  this->impl->setData(data, size);
}

unsigned char* Buffer::BufferImpl::getData() const { return this->data; }

uint32_t Buffer::BufferImpl::getSize() const { return this->size; }

void Buffer::BufferImpl::resize(uint32_t size) {
  if (this->capacity >= size) {
    this->size = size;
    return;
  }

  uint32_t cap = size * BUF_INC_RATIO;
  auto newData = new unsigned char[cap];
  if (nullptr == newData) {
    throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
  }
  memcpy(newData, this->data, this->size);

  if (NULL != this->data) delete[] this->data;
  this->capacity = cap;
  this->data = newData;
  this->size = size;
}

void Buffer::BufferImpl::clear() {
  if (NULL != this->data) {
    delete[] this->data;
    this->data = NULL;
    this->size = 0;
    this->capacity = 0;

    this->resetReadPos();
  }
}

bool Buffer::BufferImpl::isEmpty() const { return (NULL == this->data); }

String Buffer::BufferImpl::toString() const {
  return String((const char*)this->getData(), this->getSize());
}

String Buffer::BufferImpl::toHexString() {
  return ToHexString(this->getData(), this->getSize());
}

String Buffer::BufferImpl::toBinaryString() {
  stringstream str;
  for (uint32_t i = 0; i < this->getSize(); ++i) {
    for (int j = 7; j >= 0; --j) {
      str << ((this->data[i] >> j) & 0x01);
    }
    str << " ";
  }
  return str.str();
}

void Buffer::BufferImpl::fromHexString(const String& hexStr) {
  this->clear();

  auto parseLine = [&](String& s) {
    s = s.replaceAll(" ", "").replaceAll("0x", "");
    size_t len = s.length() / 2;
    for (size_t i = 0; i < len; ++i) {
      stringstream str;
      str << hex << string(s.c_str() + i * 2, 2);
      uint32_t c = 0;
      str >> hex >> c;
      this->append(&c, 1);
    }
  };

  stringstream str;
  str << hexStr.trim().toLower();
  while (!str.eof()) {
    String line;
    std::getline(str, line);
    parseLine(line);
  }
}

/* end of Buffer::BufferImpl */

NS_FF_END
