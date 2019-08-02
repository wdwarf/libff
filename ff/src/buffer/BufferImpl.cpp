/*
 * BufferImpl.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include "BufferImpl.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

using namespace std;

namespace {

const float BUF_INC_RATIO = 1.5;

}

namespace NS_FF {

Buffer::BufferImpl::BufferImpl() :
		data(NULL), size(0), capacity(0), readPos(0) {

}

Buffer::BufferImpl::BufferImpl(unsigned int initSize) :
		data(NULL), size(0), capacity(0), readPos(0) {
	this->alloc(initSize);
}

Buffer::BufferImpl::BufferImpl(const char* data, unsigned int size) :
		data(NULL), size(0), capacity(0), readPos(0) {
	this->setData(data, size);
}

Buffer::BufferImpl::BufferImpl(const Buffer::BufferImpl& buffer) :
		data(NULL), size(0), capacity(0), readPos(0) {
	this->setData(buffer.getData(), buffer.getSize());
}

int Buffer::BufferImpl::read(void* buf, unsigned int size) const {
	long avaliableSize = this->getSize() - this->readPos;
	if (avaliableSize <= 0)
		return 0;

	unsigned int readSzie = avaliableSize > size ? size : avaliableSize;
	memcpy(buf, this->data + this->readPos, readSzie);
	this->readPos += readSzie;
	return readSzie;
}

void Buffer::BufferImpl::zero() {
	if (this->capacity > 0) {
		memset(this->data, 0, this->capacity);
	}
}

void Buffer::BufferImpl::resetReadPos() const {
	this->readPos = 0;
}

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

void Buffer::BufferImpl::append(const char* data, unsigned int size) {
	if ((NULL == data) || (size <= 0))
		return;

	unsigned int reserveSize = this->capacity - this->size;
	if (size <= reserveSize) {
		memcpy(this->data + this->size, data, size);
		this->size += size;
		return;
	}

	this->capacity = (this->size + size) * BUF_INC_RATIO;
	char* newData = new char[this->capacity];
	memcpy(newData, this->data, this->size);
	memcpy(newData + this->size, data, size);

	if (NULL != this->data)
		delete[] this->data;
	this->data = newData;
	this->size += size;
}

void Buffer::BufferImpl::append(const BufferImpl& buffer) {
	this->append(buffer.data, buffer.size);
}

void Buffer::ReverseBytes(char* buf, int size) {
	if (!buf || (size <= 0)) {
		return;
	}

	char* pH = buf;
	char* pE = buf + size - 1;
	while (pH < pE) {
		char c = *pH;
		*pH = *pE;
		*pE = c;
		++pH;
		--pE;
	}
}

void Buffer::BufferImpl::reverse() {
	Buffer::ReverseBytes(this->data, this->size);

	this->resetReadPos();
}

void Buffer::BufferImpl::alloc(unsigned int size) {
	this->clear();

	if (size <= 0) {
		return;
	}

	this->size = size;
	this->capacity = this->size * BUF_INC_RATIO;
	this->data = new char[this->capacity];
	if (!this->data) {
		this->size = 0;
		throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
	}
	this->zero();

	this->resetReadPos();
}

unsigned int Buffer::BufferImpl::getCapacity() const {
	return this->capacity;
}

char& Buffer::BufferImpl::operator[](unsigned index) {
	if (index >= this->size) {
		THROW_EXCEPTION(BufferException, "Index out of bound.", this->size);
	}
	return this->data[index];
}

const char& Buffer::BufferImpl::operator[](unsigned index) const {
	if (index >= this->size) {
		THROW_EXCEPTION(BufferException, "Index out of bound.", this->size);
	}
	return this->data[index];
}

void Buffer::BufferImpl::setData(const char* data, unsigned int size) {
	char* oldData = this->data;
	if ((NULL != data) && (size > 0)) {
		this->size = size;
		this->capacity = this->size * BUF_INC_RATIO;
		this->data = new char[this->capacity];
		if (!this->data) {
			this->size = 0;
			throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
		}
		memcpy(this->data, data, this->size);
	} else {
		this->size = 0;
		this->data = NULL;
		this->capacity = 0;
	}

	this->resetReadPos();

	if (oldData)
		delete[] oldData;
}

Buffer::BufferImpl::~BufferImpl() {
	this->clear();
}

void Buffer::setData(const char* data, unsigned int size) {
	this->impl->setData(data, size);
}

char* Buffer::BufferImpl::getData() const {
	return this->data;
}

unsigned int Buffer::BufferImpl::getSize() const {
	return this->size;
}

void Buffer::BufferImpl::resize(unsigned int size) {
	if (this->capacity >= size) {
		this->size = size;
		return;
	}

	this->capacity = size * BUF_INC_RATIO;
	char* newData = new char[this->capacity];
	if (!newData) {
		this->size = 0;
		throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
	}
	memcpy(newData, this->data, size);

	if (NULL != this->data)
		delete[] this->data;
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

bool Buffer::BufferImpl::isEmpty() const {
	return (NULL == this->data);
}

string Buffer::BufferImpl::toString() {
	stringstream strBuf;
	if (!this->isEmpty()) {
		unsigned char* data = (unsigned char*) this->getData();
		unsigned int size = this->getSize();

		strBuf.fill('0');
		for (unsigned int i = 0; i < size; ++i) {
			strBuf.width(2);
			strBuf << hex << (int) data[i] << " ";
		}
	}

	return strBuf.str();
}

/* end of Buffer::BufferImpl */

} /* namespace NS_FF */
