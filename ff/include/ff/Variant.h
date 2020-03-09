/*
 * Variant.h
 *
 *  Created on: 2016-12-01
 *      Author: liyawu
 */

#ifndef FF_VARIANT_H_
#define FF_VARIANT_H_

#include <ff/Object.h>
#include <ff/Buffer.h>
#include <ff/Exception.h>
#include <string>

namespace NS_FF {

EXCEPTION_DEF(VariantException);

enum class VariantType : unsigned char {
	UNKNOWN = 0,
	BOOLEAN,
	UCHAR,
	USHORT,
	UINT,
	ULONG,
	ULONGLONG,
	CHAR,
	SHORT,
	INT,
	LONG,
	LONGLONG,
	FLOAT,
	DOUBLE,
	CARRAY,
	STRING
};

struct LIBFF_API VariantTypeInfo {
	VariantType type;
	long size;

	VariantTypeInfo() :
			type(VariantType::UNKNOWN), size(0) {
	}
};

class LIBFF_API Variant {
public:
	Variant();
	Variant(const Variant& v);
	Variant(Variant&& v);
	Variant& operator=(const Variant& v);
	Variant(bool v);
	Variant(char v);
	Variant(short v);
	Variant(int v);
	Variant(long v);
	Variant(long long v);
	Variant(unsigned char v);
	Variant(unsigned short v);
	Variant(unsigned int v);
	Variant(unsigned long v);
	Variant(unsigned long long v);
	Variant(float v);
	Variant(double v);
	Variant(const char* v);
	Variant(const std::string& v);
	Variant(const Buffer& v);
	~Variant();

	std::string toString() const;
	Buffer toBuffer() const;

	void setValue(const void* v);
	void setValue(const void* v, unsigned long size);
	void setValue(const void* v, VariantType type);
	void setValue(const void* v, const std::string& typeName);

	operator bool() const;
	operator char() const;
	operator short() const;
	operator int() const;
	operator long() const;
	operator long long() const;
	operator unsigned char() const;
	operator unsigned short() const;
	operator unsigned int() const;
	operator unsigned long() const;
	operator unsigned long long() const;
	operator float() const;
	operator double() const;
	operator std::string() const;
	operator Buffer() const;

	void clear();
	void zero();
	unsigned long getSize() const;
	void setSize(unsigned long size);
	VariantType getVt() const;
	void setVt(VariantType vt);
	void setVt(const std::string& typeName);

	static VariantTypeInfo TypeInfoFromString(const std::string& typeName);
	static VariantTypeInfo TypeInfo(VariantType type);

	bool operator==(const Variant& v) const;

	Variant operator +(const char* v) const;
	Variant operator +(const std::string& v) const;
	Variant operator ==(const bool& v) const;
	Variant operator !=(const bool& v) const;

	LIBFF_API friend std::ostream& operator<<(std::ostream& o, const Variant& v);

#define _VARIANT_OPT_DEF_(T, OP) LIBFF_API friend  Variant operator OP(const T& p1, const Variant& p2);

#define _VARIANT_OPT_DEF(OP) _VARIANT_OPT_DEF_(char, OP)\
_VARIANT_OPT_DEF_(short, OP)\
_VARIANT_OPT_DEF_(int, OP)\
_VARIANT_OPT_DEF_(long long, OP)\
_VARIANT_OPT_DEF_(unsigned char, OP)\
_VARIANT_OPT_DEF_(unsigned short, OP)\
_VARIANT_OPT_DEF_(unsigned int, OP)\
_VARIANT_OPT_DEF_(unsigned long long, OP)

	_VARIANT_OPT_DEF(+)
	_VARIANT_OPT_DEF(-)
	_VARIANT_OPT_DEF(*)
	_VARIANT_OPT_DEF(/)
	_VARIANT_OPT_DEF(%)
	_VARIANT_OPT_DEF(==)
	_VARIANT_OPT_DEF(!=)
	_VARIANT_OPT_DEF(|)
	_VARIANT_OPT_DEF(&)

#define _VARIANT_OPT_DEF_FLOAT(OP) _VARIANT_OPT_DEF_(float, OP)\
_VARIANT_OPT_DEF_(double, OP)

	_VARIANT_OPT_DEF_FLOAT(+)
	_VARIANT_OPT_DEF_FLOAT(-)
	_VARIANT_OPT_DEF_FLOAT(*)
	_VARIANT_OPT_DEF_FLOAT(/)
	_VARIANT_OPT_DEF_FLOAT(==)
	_VARIANT_OPT_DEF_FLOAT(!=)

#define _VARIANT_OPT_DEF2_(T, OP) Variant operator OP(const T& v);

#define _VARIANT_OPT_DEF2(OP) _VARIANT_OPT_DEF2_(char, OP)\
_VARIANT_OPT_DEF2_(short, OP)\
_VARIANT_OPT_DEF2_(int, OP)\
_VARIANT_OPT_DEF2_(long long, OP)\
_VARIANT_OPT_DEF2_(unsigned char, OP)\
_VARIANT_OPT_DEF2_(unsigned short, OP)\
_VARIANT_OPT_DEF2_(unsigned int, OP)\
_VARIANT_OPT_DEF2_(unsigned long long, OP)

	_VARIANT_OPT_DEF2(+)
	_VARIANT_OPT_DEF2(-)
	_VARIANT_OPT_DEF2(*)
	_VARIANT_OPT_DEF2(/)
	_VARIANT_OPT_DEF2(%)
	_VARIANT_OPT_DEF2(==)
	_VARIANT_OPT_DEF2(!=)
	_VARIANT_OPT_DEF2(|)
	_VARIANT_OPT_DEF2(&)

#define _VARIANT_OPT_DEF_FLOAT2(OP) _VARIANT_OPT_DEF2_(float, OP)\
_VARIANT_OPT_DEF2_(double, OP)

	_VARIANT_OPT_DEF_FLOAT2(+)
	_VARIANT_OPT_DEF_FLOAT2(-)
	_VARIANT_OPT_DEF_FLOAT2(*)
	_VARIANT_OPT_DEF_FLOAT2(/)
	_VARIANT_OPT_DEF_FLOAT2(==)
	_VARIANT_OPT_DEF_FLOAT2(!=)

	LIBFF_API friend Variant operator +(const char* p1, const Variant& p2);
	LIBFF_API friend Variant operator +(const std::string& p1, const Variant& p2);

	LIBFF_API friend Variant operator ==(const bool& p1, const Variant& p2);
	LIBFF_API friend Variant operator !=(const bool& p1, const Variant& p2);

private:
	VariantType vt;
	unsigned long size;
	typedef union {
		void* valPtr;
		bool valBool;
		char valChar;
		short valShort;
		int valInt;
		long valLong;
		long long valLongLong;
		unsigned char valUChar;
		unsigned short valUShort;
		unsigned int valUInt;
		unsigned long valULong;
		unsigned long long valULongLong;
		float valFloat;
		double valDouble;
	} Value;
	Value value;

private:
	template<typename T>
	T toValue() const;
};

} /* namespace NS_FF */

#endif /* FF_VARIANT_H_ */
