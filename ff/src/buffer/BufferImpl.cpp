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
#include <iomanip>
#include <algorithm>
#include <vector>
#include <ff/String.h>

using namespace std;

namespace {

const float BUF_INC_RATIO = 1.5;

}

NS_FF_BEG

Buffer::BufferImpl::BufferImpl() :
		data(NULL), size(0), capacity(0), readPos(0) {

}

Buffer::BufferImpl::BufferImpl(unsigned int initSize) :
		data(NULL), size(0), capacity(0), readPos(0) {
	this->alloc(initSize);
}

Buffer::BufferImpl::BufferImpl(const void* data, unsigned int size) :
		data(NULL), size(0), capacity(0), readPos(0) {
	this->setData(data, size);
}

Buffer::BufferImpl::BufferImpl(const Buffer::BufferImpl& buffer) :
		data(NULL), size(0), capacity(0), readPos(0) {
	this->setData(buffer.getData(), buffer.getSize());
}

int Buffer::BufferImpl::read(void* buf, unsigned int size) {
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

void Buffer::BufferImpl::resetReadPos() {
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

void Buffer::BufferImpl::attach(void* data, unsigned int size){
	this->clear();
	this->capacity = size;
	this->size = size;
	this->data = (unsigned char*)data;
	this->readPos = 0;
}

void Buffer::BufferImpl::append(const void* data, unsigned int size) {
	if ((NULL == data) || (size <= 0))
		return;

	unsigned int reserveSize = this->capacity - this->size;
	if (size <= reserveSize) {
		memcpy(static_cast<unsigned char*>(this->data) + this->size, data, size);
		this->size += size;
		return;
	}

	this->capacity = (this->size + size) * BUF_INC_RATIO;
	auto newData = new unsigned char[this->capacity];
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

String Buffer::BufferImpl::ToHexString(const void* buf, int size)
{
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

void Buffer::BufferImpl::alloc(unsigned int size) {
	this->clear();

	if (size <= 0) {
		return;
	}

	this->size = size;
	this->capacity = this->size * BUF_INC_RATIO;
	this->data = new unsigned char[this->capacity];
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

unsigned char& Buffer::BufferImpl::at(unsigned int index) {
	if (index >= this->size) {
		THROW_EXCEPTION(BufferException, "Index out of bound.", this->size);
	}
	return this->data[index];
}

const unsigned char& Buffer::BufferImpl::at(unsigned int index) const {
	if (index >= this->size) {
		THROW_EXCEPTION(BufferException, "Index out of bound.", this->size);
	}
	return this->data[index];
}

void Buffer::BufferImpl::setData(const void* data, unsigned int size) {
	auto oldData = this->data;
	if ((NULL != data) && (size > 0)) {
		this->size = size;
		this->capacity = this->size * BUF_INC_RATIO;
		this->data = new unsigned char[this->capacity];
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

void Buffer::setData(const void* data, unsigned int size) {
	this->impl->setData(data, size);
}

unsigned char* Buffer::BufferImpl::getData() const {
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
	auto newData = new unsigned char[this->capacity];
	if (!newData) {
		this->size = 0;
		throw MK_EXCEPTION(BufferException, "Alloc buffer failed", size);
	}
	memcpy(newData, this->data, this->size);

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

String Buffer::BufferImpl::toHexString() {
	return ToHexString(this->getData(), this->getSize());
}

String Buffer::BufferImpl::toBinaryString() {
	stringstream str;
	for (unsigned int i = 0; i < this->getSize(); ++i) {
		for (int j = 7; j >= 0; --j) {
			str << ((this->data[i] >> j) & 0x01);
		}
		str << " ";
	}
	return str.str();
}

void Buffer::BufferImpl::fromHexString(const String& hexStr){
	this->clear();

	auto parseLine = [&](String& s){
		s = s.replaceAll(" ", "").replaceAll("0x", "");
		size_t len = s.length() / 2;
		for(size_t i = 0; i < len; ++i){
			stringstream str;
			str << hex << string(s.c_str() + i * 2, 2);
			unsigned int c = 0;
			str >> hex >> c;
			this->append(&c, 1);
		}
	};

	stringstream str;
	str << hexStr.trim().toLower();
	while(!str.eof()){
		String line;
		std::getline(str, line);
		parseLine(line);
	}
}

/* end of Buffer::BufferImpl */

NS_FF_END
