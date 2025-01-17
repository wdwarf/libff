/*
 * Buffer.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_BUFFER_H_
#define FF_BUFFER_H_

#include <ff/Exception.h>
#include <ff/Object.h>
#include <ff/String.h>

#include <memory>
#include <sstream>
#include <string>

NS_FF_BEG

EXCEPTION_DEF(BufferException);

#define FF_BUF_IN_OPERATOR_DEF(T) Buffer& operator<<(const T& t)
#define FF_BUF_OUT_OPERATOR_DEF(T) Buffer& operator>>(T& t)

class LIBFF_API Buffer {
 public:
  Buffer();
  Buffer(uint32_t initSize);
  Buffer(const void* data, uint32_t size);
  Buffer(const Buffer& buffer);
  Buffer(Buffer&& buffer);
  ~Buffer();

  Buffer& operator=(const Buffer& buffer);
  Buffer& operator=(Buffer&& buffer);
  Buffer& operator+=(const Buffer& buffer);
  Buffer operator+(const Buffer& buffer) const;
  bool operator==(const Buffer& buffer);
  unsigned char& at(uint32_t index);
  unsigned char& at(uint32_t index) const;
  unsigned char& operator[](uint32_t index);
  const unsigned char& operator[](uint32_t index) const;

  void attach(void* data, uint32_t size);

  void append(const void* data, uint32_t size);
  void append(const Buffer& buffer);

  void setData(const void* data, uint32_t size);
  unsigned char* getData() const;
  uint32_t getSize() const;
  void resize(uint32_t size);
  void clear();
  bool isEmpty() const;
  Buffer& reverse();
  void alloc(uint32_t size);
  uint32_t getCapacity() const;
  void setCapacity(uint32_t capacity);
  void zero();

  String toString() const;
  String toHexString() const;
  String toBinaryString() const;
  void fromHexString(const String& hexStr);
  int read(void* buf, uint32_t size);
  void resetReadPos();

  /** append */
  FF_BUF_IN_OPERATOR_DEF(long long);
  FF_BUF_IN_OPERATOR_DEF(long);
  FF_BUF_IN_OPERATOR_DEF(int);
  FF_BUF_IN_OPERATOR_DEF(short);
  FF_BUF_IN_OPERATOR_DEF(char);
  FF_BUF_IN_OPERATOR_DEF(unsigned long long);
  FF_BUF_IN_OPERATOR_DEF(unsigned long);
  FF_BUF_IN_OPERATOR_DEF(uint32_t);
  FF_BUF_IN_OPERATOR_DEF(unsigned short);
  FF_BUF_IN_OPERATOR_DEF(unsigned char);
  FF_BUF_IN_OPERATOR_DEF(float);
  FF_BUF_IN_OPERATOR_DEF(double);

  /** read */
  FF_BUF_OUT_OPERATOR_DEF(long long);
  FF_BUF_OUT_OPERATOR_DEF(long);
  FF_BUF_OUT_OPERATOR_DEF(int);
  FF_BUF_OUT_OPERATOR_DEF(short);
  FF_BUF_OUT_OPERATOR_DEF(char);
  FF_BUF_OUT_OPERATOR_DEF(unsigned long long);
  FF_BUF_OUT_OPERATOR_DEF(unsigned long);
  FF_BUF_OUT_OPERATOR_DEF(uint32_t);
  FF_BUF_OUT_OPERATOR_DEF(unsigned short);
  FF_BUF_OUT_OPERATOR_DEF(unsigned char);
  FF_BUF_OUT_OPERATOR_DEF(float);
  FF_BUF_OUT_OPERATOR_DEF(double);

  Buffer& operator<<(const Buffer& in_buffer);
  Buffer& operator<<(std::istream& i);
  Buffer& operator<<(const String& s);

  friend LIBFF_API std::ostream& operator<<(std::ostream& o,
                                            const Buffer& buffer);
  static void ReverseBytes(void* buf, int size);
  static String ToHexString(const void* buf, int size,
                            const std::string& separator = " ");

 private:
  unsigned char* data;
  uint32_t size;
  uint32_t capacity;
  uint32_t readPos;
};
/* class Buffer */

typedef std::shared_ptr<Buffer> BufferPtr;

NS_FF_END

#endif /* FF_BUFFER_H_ */
