/*
 * Endian.h
 *
 *  Created on: 2017年11月14日
 *      Author: liyawu
 */

#ifndef FF_ENDIAN_H_
#define FF_ENDIAN_H_

#include <ff/Buffer.h>

namespace NS_FF {

enum class EndianType {
	BIG, SMALL
};

#define __REVERSE_T T v = val;\
Buffer::ReverseBytes((char*) &v, sizeof(T));\
return v;

class FFDLL Endian {
public:
	Endian(EndianType _endianType = GetHostEndian()) :
			endianType(_endianType) {
	}

	~Endian() {
	}

	EndianType getEndianType() const;
	operator EndianType() const;
	void setEndianType(EndianType endianType);
	bool isDifferenceEndian() const;

	template<typename T> T toBig(const T& val) const {
		if (EndianType::BIG == endianType) {
			return val;
		}
		__REVERSE_T
	}

	template<typename T> T toSmall(const T& val) const {
		if (EndianType::SMALL == endianType) {
			return val;
		}
		__REVERSE_T
	}

	template<typename T> T toHost(const T& val) const {
		if (GetHostEndian() == endianType) {
			return val;
		}
		__REVERSE_T
	}

	template<typename T> T to(const T& val, const Endian& e) const {
		if (e.endianType == endianType) {
			return val;
		}
		__REVERSE_T
	}

	static Endian GetHostEndian();
	static Endian GetDefEndian();

private:
	EndianType endianType;
};

} /* namespace NS_FF */

#endif /* FF_ENDIAN_H_ */
