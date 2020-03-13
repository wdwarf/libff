/*
 * String.h
 *
 *  Created on: Sep 5, 2016
 *      Author: ducky
 */

#ifndef FF_STRING_H_
#define FF_STRING_H_

#include <ff/Object.h>
#include <string>
#include <vector>
#include <ostream>

using std::string;

namespace NS_FF {

LIBFF_API std::string& ToLower(std::string& str);
LIBFF_API std::string ToLowerCopy(const std::string& str);
LIBFF_API std::string& ToUpper(std::string& str);
LIBFF_API std::string ToUpperCopy(const std::string& str);
LIBFF_API std::string& TrimLeft(std::string& str);
LIBFF_API std::string TrimLeftCopy(const std::string& str);
LIBFF_API std::string& TrimRight(std::string& str);
LIBFF_API std::string TrimRightCopy(const std::string& str);
LIBFF_API std::string& Trim(std::string& str);
LIBFF_API std::string TrimCopy(const std::string& str);

LIBFF_API std::string& Replace(std::string& src, const std::string& find,
		const std::string& replace, bool ignoreCase = false);
LIBFF_API std::string ReplaceCopy(const std::string& src, const std::string& find,
		const std::string& replace, bool ignoreCase = false);
LIBFF_API std::string& ReplaceAll(std::string& src, const std::string& find,
		const std::string& replace, bool ignoreCase = false);
LIBFF_API std::string ReplaceAllCopy(const std::string& src, const std::string& find,
		const std::string& replace, bool ignoreCase = false);

LIBFF_API int IndexOf(const std::string& src, const std::string& find, bool ignoreCase =
		false);

/** 十六进制字符转数字 */
LIBFF_API unsigned int HexAToI(char x);
LIBFF_API char IToHexA(unsigned int x);

enum StringCompressType {
	WithEmptyString, RemoveEmptyString
};

class LIBFF_API IDelimiter {
public:
	IDelimiter(const std::string& delimiterStr);
	virtual ~IDelimiter();

	virtual int Find(const std::string& str) const = 0;
	virtual int DelimiterSize() const = 0;

protected:
	std::string m_delimiterStr;
};

class LIBFF_API IsAnyOf: public IDelimiter {
public:
	IsAnyOf(const std::string& delimiter);

	int Find(const std::string& str) const;
	int DelimiterSize() const;
};

class LIBFF_API IsStringOf: public IDelimiter {
public:
	IsStringOf(const std::string& delimiter);

	int Find(const std::string& str) const;
	int DelimiterSize() const;
};

LIBFF_API std::vector<std::string> Split(const std::string& text,
		const IDelimiter& delimiterChecker, StringCompressType compressType =
				WithEmptyString);

class LIBFF_API String : public std::string{
public:
	String() = default;
	~String() = default;

	template<typename T>
	String(const T& str) : std::string(str){}

	String(const String& str) : std::string(str){}
	String& operator=(const String& str){
		this->std::string::operator =(str);
		return *this;
	}

	String(String&& str) : std::string(move(str)){}
	String& operator=(String&& str){
		this->std::string::operator =(move(str));
		return *this;
	}

	String(const char* str, size_t len);

	bool equals(const String& str, bool isEgnoreCase = false) const;

	String trimLeft() const;
	String trimRight() const;
	String trim() const;
	String toLower() const;
	String toUpper() const;
	String replace(const String& find, const String& replace,
			bool ignoreCase = false) const;
	String replaceAll(const String& find, const String& replace,
				bool ignoreCase = false) const;
	int indexOf(const String& find, bool ignoreCase = false) const;
	std::vector<String> split(const IDelimiter& delimiterChecker,
			StringCompressType compressType = WithEmptyString);
};

} /* namespace NS_FF */

#endif /* DUCKY_STRIING_STRING_H_ */
