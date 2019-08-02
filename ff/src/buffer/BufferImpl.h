/*
 * BufferImpl.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef BUFFER_BUFFERIMPL_H_
#define BUFFER_BUFFERIMPL_H_

#include <ff/Buffer.h>

namespace NS_FF {

class Buffer::BufferImpl {
public:
	BufferImpl();
	BufferImpl(unsigned int initSize);
	BufferImpl(const char* data, unsigned int size);
	BufferImpl(const BufferImpl& buffer);
	virtual ~BufferImpl();

	BufferImpl& operator=(const BufferImpl& buffer);
	BufferImpl operator+(const BufferImpl& buffer) const;
	char& operator[](unsigned index);
	const char& operator[](unsigned index) const;

	void append(const char* data, unsigned int size);
	void append(const BufferImpl& buffer);

	void setData(const char* data, unsigned int size);
	char* getData() const;
	unsigned int getSize() const;
	void resize(unsigned int size);
	void clear();
	bool isEmpty() const;
	void reverse();
	void alloc(unsigned int size);
	unsigned int getCapacity() const;
	void zero();

	std::string toString();
	int read(void* buf, unsigned int size) const;
	void resetReadPos() const;

private:
	char* data;
	unsigned int size;
	unsigned int capacity;
	mutable unsigned int readPos;
};

} /* namespace NS_FF */

#endif /* BUFFER_BUFFERIMPL_H_ */
