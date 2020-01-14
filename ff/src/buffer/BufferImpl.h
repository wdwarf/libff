/*
 * BufferImpl.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef BUFFER_BUFFERIMPL_H_
#define BUFFER_BUFFERIMPL_H_

#include <ff/Buffer.h>
#include <ff/String.h>

namespace NS_FF {

class Buffer::BufferImpl {
public:
	BufferImpl();
	BufferImpl(unsigned int initSize);
	BufferImpl(const void* data, unsigned int size);
	BufferImpl(const BufferImpl& buffer);
	virtual ~BufferImpl();

	BufferImpl& operator=(const BufferImpl& buffer);
	BufferImpl operator+(const BufferImpl& buffer) const;
	unsigned char& at(unsigned int index);
	const unsigned char& at(unsigned int index) const;

	void attach(void* data, unsigned int size);

	void append(const void* data, unsigned int size);
	void append(const BufferImpl& buffer);

	void setData(const void* data, unsigned int size);
	unsigned char* getData() const;
	unsigned int getSize() const;
	void resize(unsigned int size);
	void clear();
	bool isEmpty() const;
	void reverse();
	void alloc(unsigned int size);
	unsigned int getCapacity() const;
	void zero();

	String toHexString();
	void fromHexString(const String& hexStr);
	int read(void* buf, unsigned int size);
	void resetReadPos();

	static void ReverseBytes(void* buf, int size);
	static String ToHexString(const void* buf, int size);
private:
	unsigned char* data;
	unsigned int size;
	unsigned int capacity;
	unsigned int readPos;
};

} /* namespace NS_FF */

#endif /* BUFFER_BUFFERIMPL_H_ */
