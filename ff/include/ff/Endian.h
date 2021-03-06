/*
 * Endian.h
 *
 *  Created on: 2017年11月14日
 *      Author: liyawu
 */

#ifndef FF_ENDIAN_H_
#define FF_ENDIAN_H_

#include <ff/Buffer.h>

NS_FF_BEG

enum class EndianType {
	Big, Small
};

#define __REVERSE_T T v = val;\
Buffer::ReverseBytes((char*) &v, sizeof(T));\
return v;

class LIBFF_API Endian {
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
		if (EndianType::Big == endianType) {
			return val;
		}
		__REVERSE_T
	}

	template<typename T> T toSmall(const T& val) const {
		if (EndianType::Small == endianType) {
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

NS_FF_END

#endif /* FF_ENDIAN_H_ */
