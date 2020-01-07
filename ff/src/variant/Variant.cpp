/*
 * Variant.cpp
 *
 *  Created on: 2016-12-01
 *      Author: liyawu
 */

#include <ff/Variant.h>
#include <ff/String.h>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstring>
#include <cmath>

using namespace std;

namespace NS_FF {

Variant::Variant() :
		vt(VariantType::UNKNOWN), size(0) {
	memset(&this->value, 0, sizeof(this->value));
}

Variant::Variant(const Variant& v) {
	this->vt = v.vt;
	this->size = v.size;
	this->value = v.value;
	if (VariantType::CARRAY == this->vt || VariantType::STRING == this->vt) {
		this->value.valPtr = new char[this->size];
		memcpy(this->value.valPtr, v.value.valPtr, this->size);
	}
}

Variant::Variant(Variant&& v) {
	this->vt = v.vt;
	this->size = v.size;
	this->value = v.value;
	v.vt = VariantType::UNKNOWN;
}

Variant::Variant(bool v) {
	this->vt = VariantType::BOOLEAN;
	this->size = TypeInfo(this->vt).size;
	this->value.valBool = v;
}

Variant::Variant(char v) {
	this->vt = VariantType::CHAR;
	this->size = TypeInfo(this->vt).size;
	this->value.valChar = v;
}

Variant::Variant(short v) {
	this->vt = VariantType::SHORT;
	this->size = TypeInfo(this->vt).size;
	this->value.valShort = v;
}

Variant::Variant(int v) {
	this->vt = VariantType::INT;
	this->size = TypeInfo(this->vt).size;
	this->value.valInt = v;
}

Variant::Variant(long v) {
	this->vt = VariantType::INT;
	this->size = TypeInfo(this->vt).size;
	this->value.valInt = v;
}

Variant::Variant(long long v) {
	this->vt = VariantType::LONGLONG;
	this->size = TypeInfo(this->vt).size;
	this->value.valLongLong = v;
}

Variant::Variant(unsigned char v) {
	this->vt = VariantType::UCHAR;
	this->size = TypeInfo(this->vt).size;
	this->value.valUChar = v;
}

Variant::Variant(unsigned short v) {
	this->vt = VariantType::USHORT;
	this->size = TypeInfo(this->vt).size;
	this->value.valUShort = v;
}

Variant::Variant(unsigned int v) {
	this->vt = VariantType::UINT;
	this->size = TypeInfo(this->vt).size;
	this->value.valUInt = v;
}

Variant::Variant(unsigned long v) {
	this->vt = VariantType::UINT;
	this->size = TypeInfo(this->vt).size;
	this->value.valUInt = v;
}

Variant::Variant(unsigned long long v) {
	this->vt = VariantType::LONGLONG;
	this->size = TypeInfo(this->vt).size;
	this->value.valULongLong = v;
}

Variant::Variant(float v) {
	this->vt = VariantType::FLOAT;
	this->size = TypeInfo(this->vt).size;
	this->value.valFloat = v;
}

Variant::Variant(double v) {
	this->vt = VariantType::DOUBLE;
	this->size = TypeInfo(this->vt).size;
	this->value.valDouble = v;
}

Variant::Variant(const char* v) {
	this->vt = VariantType::STRING;
	this->size = strlen(v);
	if (this->size <= 0) {
		memset(&this->value, 0, sizeof(this->value));
		return;
	}
	this->value.valPtr = new char[this->size];
	memcpy(this->value.valPtr, v, this->size);
}

Variant::Variant(const string& v) {
	this->vt = VariantType::STRING;
	this->size = v.length();
	if (this->size <= 0) {
		memset(&this->value, 0, sizeof(this->value));
		return;
	}
	this->value.valPtr = new char[this->size];
	memcpy(this->value.valPtr, v.c_str(), this->size);
}

Variant::Variant(const Buffer& v) {
	this->vt = VariantType::CARRAY;
	if (!v.isEmpty()) {
		this->size = v.getSize();
		this->value.valPtr = new char[this->size];
		memcpy(this->value.valPtr, v.getData(), this->size);
	} else {
		this->size = 0;
		memset(&this->value, 0, sizeof(this->value));
	}
}

Variant::~Variant() {
	this->clear();
}

Variant& Variant::operator=(const Variant& v) {
	this->vt = v.vt;
	this->size = v.size;
	this->value = v.value;
	if ((VariantType::CARRAY == this->vt || VariantType::STRING == this->vt)
			&& (this->size > 0)) {
		this->value.valPtr = new char[this->size];
		memcpy(this->value.valPtr, v.value.valPtr, this->size);
	}

	return *this;
}

VariantTypeInfo Variant::TypeInfoFromString(const string& typeName) {
	VariantTypeInfo typeInfo;
	string type = typeName;
	Trim(type);
	ToLower(type);
	if ("boolean" == type || "bool" == type) {
		return TypeInfo(VariantType::BOOLEAN);
	}

	if ("uchar" == type || "uint8" == type || "unsigned char" == type) {
		return TypeInfo(VariantType::UCHAR);
	}

	if ("ushort" == type || "uint16" == type || "unsigned short" == type) {
		return TypeInfo(VariantType::USHORT);
	}

	if ("uint" == type || "uint32" == type || "unsigned" == type
			|| "unsigned int" == type) {
		return TypeInfo(VariantType::UINT);
	}

	if ("ulong" == type || "unsigned long" == type) {
		return TypeInfo(VariantType::ULONG);
	}

	if ("ulonglong" == type || "uint64" == type || "unsigned long long" == type
			|| "unsigned longlong" == type) {
		return TypeInfo(VariantType::ULONGLONG);
	}

	if ("char" == type || "int8" == type) {
		return TypeInfo(VariantType::CHAR);
	}

	if ("short" == type || "uint16" == type) {
		return TypeInfo(VariantType::SHORT);
	}

	if ("int" == type || "int32" == type) {
		return TypeInfo(VariantType::INT);
	}

	if ("long" == type) {
		return TypeInfo(VariantType::LONG);
	}

	if ("long long" == type || "longlong" == type || "int64" == type) {
		return TypeInfo(VariantType::LONGLONG);
	}

	if ("string" == type) {
		typeInfo.type = VariantType::STRING;
		typeInfo.size = 0;
		return typeInfo;
	}

	if ((0 == type.find("char[")) && (']' == type[type.length() - 1])) {
		typeInfo.type = VariantType::CARRAY;
		stringstream strLen;
		strLen << type.substr(5, type.length() - 6);
		strLen >> typeInfo.size;
	}

	return typeInfo;
}

VariantTypeInfo Variant::TypeInfo(VariantType type) {
	VariantTypeInfo typeInfo;

	typeInfo.type = type;
	typeInfo.size =
			VariantType::BOOLEAN == type ? sizeof(bool) :
			VariantType::UCHAR == type ? sizeof(unsigned char) :
			VariantType::USHORT == type ? sizeof(unsigned short) :
			VariantType::UINT == type ? sizeof(unsigned int) :
			VariantType::ULONG == type ? sizeof(unsigned long) :
			VariantType::ULONGLONG == type ? sizeof(unsigned long long) :
			VariantType::CHAR == type ? sizeof(char) :
			VariantType::SHORT == type ? sizeof(short) :
			VariantType::INT == type ? sizeof(int) :
			VariantType::LONG == type ? sizeof(long) :
			VariantType::LONGLONG == type ? sizeof(long long) :
			VariantType::FLOAT == type ? sizeof(float) :
			VariantType::DOUBLE == type ? sizeof(double) : 0;

	return typeInfo;
}

template<typename T>
static string _ToString(const T& t) {
	stringstream val;
	val << fixed << setprecision(numeric_limits<T>::digits10) << t;
	return val.str();
}

string Variant::toString() const {
	if (this->vt == VariantType::CARRAY || this->vt == VariantType::STRING) {
		stringstream val;
		if (this->value.valPtr && this->size > 0) {
			val.write((const char*) this->value.valPtr, this->size);
		}
		return val.str();
	}

	return this->vt == VariantType::BOOLEAN ? to_string(this->value.valBool)
			: this->vt == VariantType::UCHAR ? to_string(this->value.valUChar)
			: this->vt == VariantType::USHORT ? to_string(this->value.valUShort)
			: this->vt == VariantType::UINT ? to_string(this->value.valUInt)
			: this->vt == VariantType::ULONG ? to_string(this->value.valULong)
			: this->vt == VariantType::ULONGLONG ? to_string(this->value.valULongLong)
			: this->vt == VariantType::CHAR ? to_string(this->value.valChar)
			: this->vt == VariantType::SHORT ? to_string(this->value.valShort)
			: this->vt == VariantType::INT ? to_string(this->value.valInt)
			: this->vt == VariantType::LONG ? to_string(this->value.valLong)
			: this->vt == VariantType::LONGLONG ? to_string(this->value.valLongLong)
			: this->vt == VariantType::FLOAT ? _ToString(this->value.valFloat)
			: this->vt == VariantType::DOUBLE ? _ToString(this->value.valDouble)
			: "";
}

Buffer Variant::toBuffer() const {
	if(this->vt == VariantType::UNKNOWN)
		return Buffer();

	if (this->vt == VariantType::CARRAY || this->vt == VariantType::STRING) {
		Buffer buf;
		if (this->value.valPtr && this->size > 0)
			buf.setData((const char*) this->value.valPtr, this->size);
		return buf;
	}

	return Buffer((const char*) &this->value, this->size);
}

void Variant::clear() {
	switch (this->vt) {
	case VariantType::CARRAY:
	case VariantType::STRING: {
		if (this->value.valPtr) {
			delete[] (char*) this->value.valPtr;
		}
		break;
	}
	default:
		break;
	}
	this->vt = VariantType::UNKNOWN;
	this->size = 0;
	memset(&this->value, 0, sizeof(this->value));
}

void Variant::zero() {
	if (this->vt == VariantType::CARRAY || this->vt == VariantType::STRING) {
		if (this->value.valPtr) {
			memset(this->value.valPtr, 0, this->size);
		}
		return;
	}

	memset(&this->value, 0, sizeof(this->value));
}

#define _OP_ASIGN_(valType, val)	this->clear();\
this->vt = valType;\
this->size = TypeInfo(this->vt).size;\
this->value.val = v;\
return *this

Variant& Variant::operator=(bool v) {
	_OP_ASIGN_(VariantType::BOOLEAN, valBool);
}

Variant& Variant::operator=(char v) {
	_OP_ASIGN_(VariantType::CHAR, valChar);
}

Variant& Variant::operator=(short v) {
	_OP_ASIGN_(VariantType::SHORT, valShort);
}

Variant& Variant::operator=(int v) {
	_OP_ASIGN_(VariantType::INT, valInt);
}

Variant& Variant::operator=(long v) {
	_OP_ASIGN_(VariantType::LONG, valLong);
}

Variant& Variant::operator=(long long v) {
	_OP_ASIGN_(VariantType::LONGLONG, valLongLong);
}

Variant& Variant::operator=(unsigned char v) {
	_OP_ASIGN_(VariantType::UCHAR, valUChar);
}

Variant& Variant::operator=(unsigned short v) {
	_OP_ASIGN_(VariantType::USHORT, valUShort);
}

Variant& Variant::operator=(unsigned int v) {
	_OP_ASIGN_(VariantType::UINT, valUInt);
}

Variant& Variant::operator=(unsigned long v) {
	_OP_ASIGN_(VariantType::ULONG, valULong);
}

Variant& Variant::operator=(unsigned long long v) {
	_OP_ASIGN_(VariantType::ULONGLONG, valULongLong);
}

Variant& Variant::operator=(float v) {
	_OP_ASIGN_(VariantType::FLOAT, valFloat);
}

Variant& Variant::operator=(double v) {
	_OP_ASIGN_(VariantType::DOUBLE, valDouble);
}

Variant& Variant::operator=(const char* v) {
	this->clear();
	this->vt = VariantType::STRING;
	this->size = strlen(v);
	if (this->size <= 0) {
		return *this;
	}
	this->value.valPtr = new char[this->size];
	memcpy(this->value.valPtr, v, this->size);
	return *this;
}

Variant& Variant::operator=(const string& v) {
	this->operator =(v.c_str());
	return *this;
}

Variant& Variant::operator=(const Buffer& v) {
	this->clear();
	this->vt = VariantType::CARRAY;
	if (!v.isEmpty()) {
		this->size = v.getSize();
		this->value.valPtr = new char[this->size];
		memcpy(this->value.valPtr, v.getData(), this->size);
	}
	return *this;
}

unsigned long Variant::getSize() const {
	return size;
}

void Variant::setSize(unsigned long size) {
	if ((VariantType::CARRAY == this->vt || VariantType::STRING == this->vt)
			&& (this->size != size)) {
		VariantType t = this->vt;
		this->clear();
		this->vt = t;
		this->size = size;
		if (this->size <= 0) {
			return;
		}
		this->value.valPtr = new char[this->size];
		memset(this->value.valPtr, 0, this->size);
	}
}

VariantType Variant::getVt() const {
	return vt;
}

void Variant::setVt(VariantType vt) {
	if (this->vt != vt) {
		this->clear();
	}
	this->vt = vt;
	this->size = Variant::TypeInfo(this->vt).size;
}

void Variant::setVt(const string& typeName) {
	this->clear();
	VariantTypeInfo typeInfo = TypeInfoFromString(typeName);
	this->vt = typeInfo.type;
	this->size = typeInfo.size;
}

void Variant::setValue(const void* v) {
	switch (this->vt) {
	case VariantType::UNKNOWN:
		break;
	case VariantType::CARRAY: {
		memcpy(this->value.valPtr, v, this->size);
		break;
	}
	case VariantType::STRING: {
		if (strlen((const char*) v) != this->size) {
			this->setSize(strlen((const char*) v));
		}
		memcpy(this->value.valPtr, v, this->size);
		break;
	}
	default: {
		memcpy(&this->value, v, this->size);
	}
	}
}

void Variant::setValue(const void* v, unsigned long size) {
	if (VariantType::UNKNOWN == this->vt)
		return;

	unsigned int s = min(this->size, size);
	this->zero();

	switch (this->vt) {
	case VariantType::CARRAY: {
		memcpy(this->value.valPtr, v, s);
		break;
	}
	case VariantType::STRING: {
		if (size != this->size) {
			this->setSize(size);
		}
		if (this->size > 0)
			memcpy(this->value.valPtr, v, this->size);
		break;
	}
	default: {
		memcpy(&this->value, v, s);
		break;
	}
	}
}

void Variant::setValue(const void* v, VariantType type) {
	this->setVt(type);
	this->setValue(v);
}

void Variant::setValue(const void* v, const string& typeName) {
	VariantTypeInfo info = TypeInfoFromString(typeName);
	this->setVt(info.type);
	this->setSize(info.size);
	this->setValue(v);
}

template<typename T>
T Variant::toValue() const {
	T val = 0;
	switch (this->vt) {
	case VariantType::UNKNOWN:
		break;
	case VariantType::BOOLEAN: {
		val = this->value.valBool;
		break;
	}
	case VariantType::UCHAR: {
		val = this->value.valUChar;
		break;
	}
	case VariantType::USHORT: {
		val = this->value.valUShort;
		break;
	}
	case VariantType::UINT: {
		val = this->value.valUInt;
		break;
	}
	case VariantType::ULONG: {
		val = this->value.valULong;
		break;
	}
	case VariantType::ULONGLONG: {
		val = this->value.valULongLong;
		break;
	}
	case VariantType::CHAR: {
		val = this->value.valChar;
		break;
	}
	case VariantType::SHORT: {
		val = this->value.valShort;
		break;
	}
	case VariantType::INT: {
		val = this->value.valInt;
		break;
	}
	case VariantType::LONG: {
		val = this->value.valLong;
		break;
	}
	case VariantType::LONGLONG: {
		val = this->value.valLongLong;
		break;
	}
	case VariantType::FLOAT: {
		val = this->value.valFloat;
		break;
	}
	case VariantType::DOUBLE: {
		val = this->value.valDouble;
		break;
	}
	case VariantType::CARRAY: {
		if (this->size > sizeof(T)) {
			memcpy(&val, this->value.valPtr, sizeof(T));
		}
		break;
	}
	case VariantType::STRING: {
		if (this->value.valPtr && this->size > 0) {
			stringstream str;
			str.write((const char*) this->value.valPtr, this->size);
			string valStr = ToLowerCopy(TrimCopy(str.str()));
			if (!valStr.empty()) {
				if ('+' == valStr[0] || '-' == valStr[0]) {
					valStr = valStr.substr(1);
				}

				if (valStr.length() > 1) {
					if ('0' == valStr[0]) {
						if ('x' == valStr[1]) {
							str >> hex;
						} else {
							str >> oct;
						}
					}
				}
			}
			str >> val;
		}
		break;
	}
	}
	return val;
}

Variant::operator bool() const {
	switch (this->vt) {
	case VariantType::UNKNOWN:
		break;
	case VariantType::BOOLEAN: {
		return this->value.valBool;
	}
	case VariantType::UCHAR: {
		return (0 != this->value.valUChar);
	}
	case VariantType::USHORT: {
		return (0 != this->value.valUShort);
	}
	case VariantType::UINT: {
		return (0 != this->value.valUInt);
	}
	case VariantType::ULONG: {
		return (0 != this->value.valULong);
	}
	case VariantType::ULONGLONG: {
		return (0 != this->value.valULongLong);
	}
	case VariantType::CHAR: {
		return (0 != this->value.valChar);
	}
	case VariantType::SHORT: {
		return (0 != this->value.valShort);
	}
	case VariantType::INT: {
		return (0 != this->value.valInt);
	}
	case VariantType::LONG: {
		return (0 != this->value.valLong);
	}
	case VariantType::LONGLONG: {
		return (0 != this->value.valLongLong);
	}
	case VariantType::FLOAT: {
		return (0 != this->value.valFloat);
	}
	case VariantType::DOUBLE: {
		return (0 != this->value.valDouble);
	}
	case VariantType::CARRAY:
	case VariantType::STRING: {
		string val = this->toString();
		if (val.empty() || "0" == val || "false" == val) {
			return false;
		} else {
			return true;
		}
		break;
	}
	}
	return false;
}

Variant::operator char() const {
	return toValue<char>();
}

Variant::operator short() const {
	return toValue<short>();
}

Variant::operator int() const {
	return toValue<int>();
}

Variant::operator long() const {
	return toValue<long>();
}

Variant::operator long long() const {
	return toValue<long long>();
}

Variant::operator unsigned char() const {
	return toValue<unsigned short>();
}

Variant::operator unsigned short() const {
	return toValue<unsigned short>();
}

Variant::operator unsigned int() const {
	return toValue<unsigned int>();
}

Variant::operator unsigned long() const {
	return toValue<unsigned long>();
}

Variant::operator unsigned long long() const {
	return toValue<unsigned long long>();
}

Variant::operator float() const {
	return toValue<float>();
}

Variant::operator double() const {
	return toValue<double>();
}

Variant::operator string() const {
	return this->toString();
}

Variant::operator Buffer() const {
	return this->toBuffer();
}

std::ostream& operator<<(std::ostream& o, const Variant& v) {
	o << v.toString();
	return o;
}

//bool Variant::operator==(const Variant& v) const
//{
//	return this->toString() == v.toString();
//}

bool Variant::operator==(const Variant& v) const {
	return this->toString() == v.toString();
}

Variant Variant::operator+(const char* v) const {
	return (this->toString() + v);
}

Variant Variant::operator+(const std::string& v) const {
	return (this->toString() + v);
}

Variant Variant::operator==(const bool& v) const {
	return ((bool) (*this) == v);
}

Variant Variant::operator!=(const bool& v) const {
	return ((bool) (*this) != v);
}

#define _VARIANT_OPT_DEF_IMPL_(T, OP) Variant operator OP(const T& p1, const Variant& p2)\
{\
	return (p1 OP (T)p2);\
}

#define _VARIANT_OPT_DEF_IMPL(OP) _VARIANT_OPT_DEF_IMPL_(char, OP)\
_VARIANT_OPT_DEF_IMPL_(short, OP)\
_VARIANT_OPT_DEF_IMPL_(int, OP)\
_VARIANT_OPT_DEF_IMPL_(long long, OP)\
_VARIANT_OPT_DEF_IMPL_(unsigned char, OP)\
_VARIANT_OPT_DEF_IMPL_(unsigned short, OP)\
_VARIANT_OPT_DEF_IMPL_(unsigned int, OP)\
_VARIANT_OPT_DEF_IMPL_(unsigned long long, OP)

_VARIANT_OPT_DEF_IMPL(+)
_VARIANT_OPT_DEF_IMPL(-)
_VARIANT_OPT_DEF_IMPL(*)
_VARIANT_OPT_DEF_IMPL(/)
_VARIANT_OPT_DEF_IMPL(%)
_VARIANT_OPT_DEF_IMPL(==)
_VARIANT_OPT_DEF_IMPL(!=)
_VARIANT_OPT_DEF_IMPL(|)
_VARIANT_OPT_DEF_IMPL(&)

#define _VARIANT_OPT_DEF_IMPL_FLOAT(OP) _VARIANT_OPT_DEF_IMPL_(float, OP)\
		_VARIANT_OPT_DEF_IMPL_(double, OP)

_VARIANT_OPT_DEF_IMPL_FLOAT(+)
_VARIANT_OPT_DEF_IMPL_FLOAT(-)
_VARIANT_OPT_DEF_IMPL_FLOAT(*)
_VARIANT_OPT_DEF_IMPL_FLOAT(/)
_VARIANT_OPT_DEF_IMPL_FLOAT(==)
_VARIANT_OPT_DEF_IMPL_FLOAT(!=)

#define _VARIANT_OPT_DEF_IMPL2_(T, OP) Variant Variant::operator OP(const T& v)\
{\
	return ((T)(*this) OP v);\
}

#define _VARIANT_OPT_DEF_IMPL2(OP) _VARIANT_OPT_DEF_IMPL2_(char, OP)\
_VARIANT_OPT_DEF_IMPL2_(short, OP)\
_VARIANT_OPT_DEF_IMPL2_(int, OP)\
_VARIANT_OPT_DEF_IMPL2_(long long, OP)\
_VARIANT_OPT_DEF_IMPL2_(unsigned char, OP)\
_VARIANT_OPT_DEF_IMPL2_(unsigned short, OP)\
_VARIANT_OPT_DEF_IMPL2_(unsigned int, OP)\
_VARIANT_OPT_DEF_IMPL2_(unsigned long long, OP)

_VARIANT_OPT_DEF_IMPL2(+)
_VARIANT_OPT_DEF_IMPL2(-)
_VARIANT_OPT_DEF_IMPL2(*)
_VARIANT_OPT_DEF_IMPL2(/)
_VARIANT_OPT_DEF_IMPL2(%)
_VARIANT_OPT_DEF_IMPL2(==)
_VARIANT_OPT_DEF_IMPL2(!=)
_VARIANT_OPT_DEF_IMPL2(|)
_VARIANT_OPT_DEF_IMPL2(&)

#define _VARIANT_OPT_DEF_IMPL_FLOAT2(OP) _VARIANT_OPT_DEF_IMPL2_(float, OP)\
		_VARIANT_OPT_DEF_IMPL2_(double, OP)

_VARIANT_OPT_DEF_IMPL_FLOAT2(+)
_VARIANT_OPT_DEF_IMPL_FLOAT2(-)
_VARIANT_OPT_DEF_IMPL_FLOAT2(*)
_VARIANT_OPT_DEF_IMPL_FLOAT2(/)
_VARIANT_OPT_DEF_IMPL_FLOAT2(==)
_VARIANT_OPT_DEF_IMPL_FLOAT2(!=)

Variant operator +(const char* p1, const Variant& p2) {
	return (p1 + p2.toString());
}

Variant operator +(const std::string& p1, const Variant& p2) {
	return (p1 + p2.toString());
}

Variant operator ==(const bool& p1, const Variant& p2) {
	return (p1 == (bool) p2);
}

Variant operator !=(const bool& p1, const Variant& p2) {
	return (p1 != (bool) p2);
}

} /* namespace NS_FF */

