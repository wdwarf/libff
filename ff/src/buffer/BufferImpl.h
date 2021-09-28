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

NS_FF_BEG

class Buffer::BufferImpl {
public:
	BufferImpl();
	BufferImpl(uint32_t initSize);
	BufferImpl(const void* data, uint32_t size);
	BufferImpl(const BufferImpl& buffer);
	virtual ~BufferImpl();

	BufferImpl& operator=(const BufferImpl& buffer);
	BufferImpl operator+(const BufferImpl& buffer) const;
	unsigned char& at(uint32_t index);
	const unsigned char& at(uint32_t index) const;

	void attach(void* data, uint32_t size);

	void append(const void* data, uint32_t size);
	void append(const BufferImpl& buffer);

	void setData(const void* data, uint32_t size);
	unsigned char* getData() const;
	uint32_t getSize() const;
	void resize(uint32_t size);
	void clear();
	bool isEmpty() const;
	void reverse();
	void alloc(uint32_t size);
	uint32_t getCapacity() const;
	void setCapacity(uint32_t capacity);
	void zero();

	String toString() const;
	String toHexString();
	String toBinaryString();
	void fromHexString(const String& hexStr);
	int read(void* buf, uint32_t size);
	void resetReadPos();

	static void ReverseBytes(void* buf, int size);
	static String ToHexString(const void* buf, int size);
private:
	unsigned char* data;
	uint32_t size;
	uint32_t capacity;
	uint32_t readPos;
};

NS_FF_END

#endif /* BUFFER_BUFFERIMPL_H_ */
