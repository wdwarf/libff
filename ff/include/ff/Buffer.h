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

namespace NS_FF {

EXCEPTION_DEF(BufferException)

#define BUF_IN_OPERATOR_DEF(T) Buffer& operator<<(const T& t)
#define BUF_OUT_OPERATOR_DEF(T) Buffer& operator>>(T& t)

class Buffer: virtual public Object {
public:
	Buffer();
	Buffer(unsigned int initSize);
	Buffer(const char* data, unsigned int size);
	Buffer(const Buffer& buffer);
	Buffer(Buffer&& buffer);
	virtual ~Buffer();

	Buffer& operator=(const Buffer& buffer);
	Buffer& operator+=(const Buffer& buffer);
	Buffer operator+(const Buffer& buffer) const;
	bool operator==(const Buffer& buffer);
	char& operator[](unsigned index);
	const char& operator[](unsigned index) const;

	void append(const char* data, unsigned int size);
	void append(const Buffer& buffer);

	void setData(const char* data, unsigned int size);
	char* getData() const;
	unsigned int getSize() const;
	void resize(unsigned int size);
	void clear();
	bool isEmpty() const;
	Buffer& reverse();
	void alloc(unsigned int size);
	unsigned int getCapacity() const;
	void zero();

	std::string toString() const;
	int read(void* buf, unsigned int size) const;
	void resetReadPos() const;

	BUF_IN_OPERATOR_DEF(long long);
	BUF_IN_OPERATOR_DEF(long);
	BUF_IN_OPERATOR_DEF(int);
	BUF_IN_OPERATOR_DEF(short);
	BUF_IN_OPERATOR_DEF(char);
	BUF_IN_OPERATOR_DEF(unsigned long long);
	BUF_IN_OPERATOR_DEF(unsigned long);
	BUF_IN_OPERATOR_DEF(unsigned int);
	BUF_IN_OPERATOR_DEF(unsigned short);
	BUF_IN_OPERATOR_DEF(unsigned char);
	BUF_IN_OPERATOR_DEF(float);
	BUF_IN_OPERATOR_DEF(double);

	BUF_OUT_OPERATOR_DEF(long long);
	BUF_OUT_OPERATOR_DEF(long);
	BUF_OUT_OPERATOR_DEF(int);
	BUF_OUT_OPERATOR_DEF(short);
	BUF_OUT_OPERATOR_DEF(char);
	BUF_OUT_OPERATOR_DEF(unsigned long long);
	BUF_OUT_OPERATOR_DEF(unsigned long);
	BUF_OUT_OPERATOR_DEF(unsigned int);
	BUF_OUT_OPERATOR_DEF(unsigned short);
	BUF_OUT_OPERATOR_DEF(unsigned char);
	BUF_OUT_OPERATOR_DEF(float);
	BUF_OUT_OPERATOR_DEF(double);

	Buffer& operator<<(const Buffer& in_buffer);
	Buffer& operator<<(std::istream& i);
	Buffer& operator<<(const std::string& s);

	friend std::ostream& operator<<(std::ostream& o, const Buffer& buffer);
	static void ReverseBytes(char* buf, int size);

private:
	class BufferImpl;
	BufferImpl* impl;
};
/* class Buffer */

typedef std::shared_ptr<Buffer> BufferPtr;

} /* namespace NS_FF */

#endif /* FF_BUFFER_H_ */