/*
 * Buffer.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Buffer.h>

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

NS_FF_BEG

static inline uint32_t CapcityAdj(uint32_t bufSize){
  if(0 == bufSize) return 0;
  if(bufSize <= 32) return (bufSize + 4);
  if(bufSize <= 4096) return (bufSize * 2);
  return (bufSize + 4096);
}

Buffer::Buffer() : data(nullptr), size(0), capacity(0), readPos(0) {}

Buffer::Buffer(uint32_t initSize) : Buffer() { this->alloc(initSize); }

Buffer::Buffer(const void* data, uint32_t size) : Buffer() {
  this->setData(data, size);
}

Buffer::Buffer(const Buffer& buffer) : Buffer() {
  this->setData(buffer.getData(), buffer.getSize());
}

Buffer::Buffer(Buffer&& buffer) : Buffer() {
  this->data = buffer.data;
  this->size = buffer.size;
  this->capacity = buffer.capacity;
  this->readPos = buffer.readPos;

  buffer.data = nullptr;
  buffer.size = 0;
  buffer.capacity = 0;
  buffer.readPos = 0;
}

Buffer& Buffer::operator=(const Buffer& buffer) {
  this->setData(buffer.getData(), buffer.getSize());
  return *this;
}

Buffer& Buffer::operator=(Buffer&& buffer) {
  this->clear();

  this->data = buffer.data;
  this->size = buffer.size;
  this->capacity = buffer.capacity;
  this->readPos = buffer.readPos;

  buffer.data = nullptr;
  buffer.size = 0;
  buffer.capacity = 0;
  buffer.readPos = 0;
  return *this;
}

Buffer& Buffer::operator+=(const Buffer& buffer) {
  this->append(buffer);
  return *this;
}

Buffer Buffer::operator+(const Buffer& buffer) const {
  Buffer newBuffer;
  newBuffer.append(*this);
  newBuffer.append(buffer);
  return newBuffer;
}

bool Buffer::operator==(const Buffer& buffer) {
  if (this == &buffer) return true;

  if (this->getSize() != buffer.getSize()) return false;

  return (0 == memcmp(this->getData(), buffer.getData(), this->getSize()));
}

void Buffer::attach(void* data, uint32_t size) {
  this->clear();
  this->capacity = size;
  this->size = size;
  this->data = (unsigned char*)data;
  this->readPos = 0;
}

void Buffer::append(const void* data, uint32_t size) {
  if ((nullptr == data) || (size <= 0)) return;

  uint32_t reserveSize = this->capacity - this->size;
  if (size <= reserveSize) {
    memcpy(static_cast<unsigned char*>(this->data) + this->size, data, size);
    this->size += size;
    return;
  }

  uint32_t cap = CapcityAdj(this->size + size);
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

void Buffer::append(const Buffer& buffer) {
  this->append(buffer.data, buffer.size);
}

unsigned char& Buffer::at(uint32_t index) {
  if (index >= this->size) {
    THROW_EXCEPTION(BufferException, "Index out of bound.", this->size);
  }
  return this->data[index];
}

unsigned char& Buffer::at(uint32_t index) const {
  if (index >= this->size) {
    THROW_EXCEPTION(BufferException, "Index out of bound.", this->size);
  }
  return this->data[index];
}

unsigned char& Buffer::operator[](uint32_t index) { return this->at(index); }

const unsigned char& Buffer::operator[](uint32_t index) const {
  return this->at(index);
}

Buffer::~Buffer() { this->clear(); }

unsigned char* Buffer::getData() const { return this->data; }

uint32_t Buffer::getSize() const { return this->size; }

void Buffer::resize(uint32_t size) {
  if (this->capacity >= size) {
    this->size = size;
    return;
  }

  uint32_t cap = CapcityAdj(size);
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

void Buffer::clear() {
  if (nullptr != this->data) {
    delete[] this->data;
    this->data = nullptr;
    this->size = 0;
    this->capacity = 0;

    this->resetReadPos();
  }
}

bool Buffer::isEmpty() const { return (NULL == this->data); }

void Buffer::setData(const void* data, uint32_t size) {
  auto oldData = this->data;
  if ((nullptr != data) && (size > 0)) {
    uint32_t cap = CapcityAdj(size);
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

String Buffer::toString() const {
  return String((const char*)this->getData(), this->getSize());
}

String Buffer::toHexString() const {
  return ToHexString(this->getData(), this->getSize());
}

String Buffer::toBinaryString() const {
  stringstream str;
  for (uint32_t i = 0; i < this->getSize(); ++i) {
    for (int j = 7; j >= 0; --j) {
      str << ((this->data[i] >> j) & 0x01);
    }
    str << " ";
  }
  return str.str();
}

void Buffer::fromHexString(const String& hexStr) {
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

Buffer& Buffer::reverse() {
  Buffer::ReverseBytes(this->data, this->size);

  this->resetReadPos();
  return *this;
}

void Buffer::alloc(uint32_t size) {
  if (size <= 0) {
    return;
  }

  if (this->capacity >= size) {
    this->zero();
    this->resetReadPos();
    return;
  }

  this->clear();

  uint32_t cap = CapcityAdj(size);
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

uint32_t Buffer::getCapacity() const { return this->capacity; }

void Buffer::setCapacity(uint32_t capacity) {
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

void Buffer::zero() {
  if (this->capacity > 0) {
    memset(this->data, 0, this->capacity);
  }
}

int Buffer::read(void* buf, uint32_t size) {
  uint32_t avaliableSize = this->getSize() - this->readPos;
  if (avaliableSize <= 0) return 0;

  uint32_t readSzie = avaliableSize > size ? size : avaliableSize;
  memcpy(buf, this->data + this->readPos, readSzie);
  this->readPos += readSzie;
  return readSzie;
}

void Buffer::resetReadPos() { this->readPos = 0; }

Buffer& Buffer::operator<<(const Buffer& in_buffer) {
  this->append(in_buffer);
  return *this;
}

Buffer& Buffer::operator<<(istream& i) {
  istream::pos_type pos = i.tellg();
  i.seekg(0, ios::end);
  istream::pos_type endPos = i.tellg();
  i.seekg(pos, ios::beg);
  istream::pos_type len = endPos - pos;
  vector<char> v(len);
  i.read(&v[0], len);
  this->append(&v[0], len);

  return *this;
}

void Buffer::ReverseBytes(void* buf, int size) {
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

String Buffer::ToHexString(const void* buf, int size) {
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

Buffer& Buffer::operator<<(const String& s) {
  this->append(s.c_str(), s.length());
  return *this;
}

#define BUF_IN_OPERATOR_IMPL(T)               \
  Buffer& Buffer::operator<<(const T& t) {    \
    this->append((const char*)&t, sizeof(T)); \
    return *this;                             \
  }

#define BUF_OUT_OPERATOR_IMPL(T)      \
  Buffer& Buffer::operator>>(T& t) {  \
    this->read((void*)&t, sizeof(t)); \
    return *this;                     \
  }

BUF_IN_OPERATOR_IMPL(long long);
BUF_IN_OPERATOR_IMPL(long);
BUF_IN_OPERATOR_IMPL(int);
BUF_IN_OPERATOR_IMPL(short);
BUF_IN_OPERATOR_IMPL(char);
BUF_IN_OPERATOR_IMPL(unsigned long long);
BUF_IN_OPERATOR_IMPL(unsigned long);
BUF_IN_OPERATOR_IMPL(uint32_t);
BUF_IN_OPERATOR_IMPL(unsigned short);
BUF_IN_OPERATOR_IMPL(unsigned char);
BUF_IN_OPERATOR_IMPL(float);
BUF_IN_OPERATOR_IMPL(double);

BUF_OUT_OPERATOR_IMPL(long long);
BUF_OUT_OPERATOR_IMPL(long);
BUF_OUT_OPERATOR_IMPL(int);
BUF_OUT_OPERATOR_IMPL(short);
BUF_OUT_OPERATOR_IMPL(char);
BUF_OUT_OPERATOR_IMPL(unsigned long long);
BUF_OUT_OPERATOR_IMPL(unsigned long);
BUF_OUT_OPERATOR_IMPL(uint32_t);
BUF_OUT_OPERATOR_IMPL(unsigned short);
BUF_OUT_OPERATOR_IMPL(unsigned char);
BUF_OUT_OPERATOR_IMPL(float);
BUF_OUT_OPERATOR_IMPL(double);

ostream& operator<<(ostream& o, const NS_FF::Buffer& buffer) {
  o.write((const char*)buffer.getData(), buffer.getSize());
  return o;
}

NS_FF_END
