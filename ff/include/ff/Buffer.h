/*
 * Buffer.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_BUFFER_H_
#define FF_BUFFER_H_

#include <string>
#include <sstream>
#include <memory>
#include <ff/Object.h>
#include <ff/Exception.h>
#include <ff/String.h>

NS_FF_BEG

EXCEPTION_DEF(BufferException);

#define BUF_IN_OPERATOR_DEF(T) Buffer& operator<<(const T& t)
#define BUF_OUT_OPERATOR_DEF(T) Buffer& operator>>(T& t)

class LIBFF_API Buffer {
public:
	Buffer();
	Buffer(uint32_t initSize);
	Buffer(const void* data, uint32_t size);
	Buffer(const Buffer& buffer);
	Buffer(Buffer&& buffer);
	virtual ~Buffer();

	Buffer& operator=(const Buffer& buffer);
	Buffer& operator=(Buffer&& buffer);
	Buffer& operator+=(const Buffer& buffer);
	Buffer operator+(const Buffer& buffer) const;
	bool operator==(const Buffer& buffer);
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
	BUF_IN_OPERATOR_DEF(long long);
	BUF_IN_OPERATOR_DEF(long);
	BUF_IN_OPERATOR_DEF(int);
	BUF_IN_OPERATOR_DEF(short);
	BUF_IN_OPERATOR_DEF(char);
	BUF_IN_OPERATOR_DEF(unsigned long long);
	BUF_IN_OPERATOR_DEF(unsigned long);
	BUF_IN_OPERATOR_DEF(uint32_t);
	BUF_IN_OPERATOR_DEF(unsigned short);
	BUF_IN_OPERATOR_DEF(unsigned char);
	BUF_IN_OPERATOR_DEF(float);
	BUF_IN_OPERATOR_DEF(double);

	/** read */
	BUF_OUT_OPERATOR_DEF(long long);
	BUF_OUT_OPERATOR_DEF(long);
	BUF_OUT_OPERATOR_DEF(int);
	BUF_OUT_OPERATOR_DEF(short);
	BUF_OUT_OPERATOR_DEF(char);
	BUF_OUT_OPERATOR_DEF(unsigned long long);
	BUF_OUT_OPERATOR_DEF(unsigned long);
	BUF_OUT_OPERATOR_DEF(uint32_t);
	BUF_OUT_OPERATOR_DEF(unsigned short);
	BUF_OUT_OPERATOR_DEF(unsigned char);
	BUF_OUT_OPERATOR_DEF(float);
	BUF_OUT_OPERATOR_DEF(double);

	Buffer& operator<<(const Buffer& in_buffer);
	Buffer& operator<<(std::istream& i);
	Buffer& operator<<(const String& s);

	friend LIBFF_API std::ostream& operator<<(std::ostream& o, const Buffer& buffer);
	static void ReverseBytes(void* buf, int size);
	static String ToHexString(const void* buf, int size);

private:
	class BufferImpl;
	BufferImpl* impl;
};
/* class Buffer */

typedef std::shared_ptr<Buffer> BufferPtr;

NS_FF_END

#endif /* FF_BUFFER_H_ */
