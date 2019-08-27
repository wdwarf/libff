/*
 * Buffer.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Buffer.h>
#include "BufferImpl.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

using namespace std;

namespace NS_FF {

Buffer::Buffer() :
		impl(new Buffer::BufferImpl()) {

}

Buffer::Buffer(unsigned int initSize) :
		impl(new Buffer::BufferImpl(initSize)) {
}

Buffer::Buffer(const void* data, unsigned int size) :
		impl(new Buffer::BufferImpl(data, size)) {
}

Buffer::Buffer(const Buffer& buffer) :
		impl(new Buffer::BufferImpl(*buffer.impl)) {
}

Buffer::Buffer(Buffer&& buffer){
	if(nullptr != this->impl)
		delete this->impl;

	this->impl = buffer.impl;
	buffer.impl = nullptr;
}

Buffer& Buffer::operator=(const Buffer& buffer) {
	*this->impl = *buffer.impl;
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
	if (this == &buffer)
		return true;

	if (this->getSize() != buffer.getSize())
		return false;

	return (0 == memcmp(this->getData(), buffer.getData(), this->getSize()));
}

void Buffer::append(const void* data, unsigned int size) {
	this->impl->append(data, size);
}

void Buffer::append(const Buffer& buffer) {
	this->impl->append(*buffer.impl);
}

char& Buffer::operator[](unsigned index) {
	return this->impl->operator [](index);
}

const char& Buffer::operator[](unsigned index) const {
	return this->impl->operator [](index);
}

Buffer::~Buffer() {
	if (nullptr != this->impl) {
		delete this->impl;
	}
}

char* Buffer::getData() const {
	return this->impl->getData();
}

unsigned int Buffer::getSize() const {
	return this->impl->getSize();
}

void Buffer::resize(unsigned int size) {
	this->impl->resize(size);
}

void Buffer::clear() {
	this->impl->clear();
}

bool Buffer::isEmpty() const {
	return this->impl->isEmpty();
}

String Buffer::toHexString() const {
	return this->impl->toHexString();
}

void Buffer::fromHexString(const String& hexStr){
	return this->impl->fromHexString(hexStr);
}

Buffer& Buffer::reverse() {
	this->impl->reverse();
	return *this;
}

void Buffer::alloc(unsigned int size) {
	this->impl->alloc(size);
}

unsigned int Buffer::getCapacity() const {
	return this->impl->getCapacity();
}

void Buffer::zero() {
	this->impl->zero();
}

int Buffer::read(void* buf, unsigned int size) {
	return this->impl->read(buf, size);
}

void Buffer::resetReadPos() {
	this->impl->resetReadPos();
}

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
	BufferImpl::ReverseBytes(buf, size);
}

String Buffer::ToHexString(void* buf, int size)
{
	return BufferImpl::ToHexString(buf, size);
}

Buffer& Buffer::operator<<(const String& s) {
	this->append(s.c_str(), s.length());
	return *this;
}

#define BUF_IN_OPERATOR_IMPL(T) Buffer& Buffer::operator<<(const T& t) {\
		this->append((const char*) &t, sizeof(T));\
		return *this;\
}

#define BUF_OUT_OPERATOR_IMPL(T) Buffer& Buffer::operator>>(T& t) {\
		this->read((void*)&t, sizeof(t));\
		return *this;\
}

BUF_IN_OPERATOR_IMPL(long long);
BUF_IN_OPERATOR_IMPL(long);
BUF_IN_OPERATOR_IMPL(int);
BUF_IN_OPERATOR_IMPL(short);
BUF_IN_OPERATOR_IMPL(char);
BUF_IN_OPERATOR_IMPL(unsigned long long);
BUF_IN_OPERATOR_IMPL(unsigned long);
BUF_IN_OPERATOR_IMPL(unsigned int);
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
BUF_OUT_OPERATOR_IMPL(unsigned int);
BUF_OUT_OPERATOR_IMPL(unsigned short);
BUF_OUT_OPERATOR_IMPL(unsigned char);
BUF_OUT_OPERATOR_IMPL(float);
BUF_OUT_OPERATOR_IMPL(double);

ostream& operator<<(ostream& o, const NS_FF::Buffer& buffer) {
	o.write(buffer.getData(), buffer.getSize());
	return o;
}

} /* namespace NS_FF */

