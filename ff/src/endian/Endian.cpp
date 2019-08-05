/*
 * Endian.cpp
 *
 *  Created on: 2017年11月14日
 *      Author: liyawu
 */

#include <ff/Endian.h>

namespace NS_FF {

Endian Endian::GetHostEndian() {
	static unsigned short n = 0x1234;
	unsigned char* buf = (unsigned char*) &n;
	if (0x34 == buf[0]) {
		return Endian(EndianType::SMALL);
	} else {
		return Endian(EndianType::BIG);
	}
}

Endian Endian::GetDefEndian() {
	switch (Endian::GetHostEndian().getEndianType()) {
	case EndianType::SMALL:
		return Endian(EndianType::BIG);
	case EndianType::BIG:
		return EndianType::SMALL;
	}
	return Endian(EndianType::SMALL);
}

EndianType Endian::getEndianType() const {
	return endianType;
}

Endian::operator EndianType() const {
	return this->getEndianType();
}

void Endian::setEndianType(EndianType endianType) {
	this->endianType = endianType;
}

bool Endian::isDifferenceEndian() const {
	return (GetHostEndian() != this->endianType);
}

} /* namespace NS_FF */

