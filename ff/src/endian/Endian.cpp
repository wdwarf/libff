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
		return Endian(EndianType::Small);
	} else {
		return Endian(EndianType::Big);
	}
}

Endian Endian::GetDefEndian() {
	switch (Endian::GetHostEndian().getEndianType()) {
	case EndianType::Small:
		return Endian(EndianType::Big);
	case EndianType::Big:
		return EndianType::Small;
	}
	return Endian(EndianType::Small);
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

