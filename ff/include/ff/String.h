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

FFDLL std::string& ToLower(std::string& str);
FFDLL std::string ToLowerCopy(const std::string& str);
FFDLL std::string& ToUpper(std::string& str);
FFDLL std::string ToUpperCopy(const std::string& str);
FFDLL std::string& TrimLeft(std::string& str);
FFDLL std::string TrimLeftCopy(const std::string& str);
FFDLL std::string& TrimRight(std::string& str);
FFDLL std::string TrimRightCopy(const std::string& str);
FFDLL std::string& Trim(std::string& str);
FFDLL std::string TrimCopy(const std::string& str);

FFDLL std::string& Replace(std::string& src, const std::string& find,
		const std::string& replace, bool ignoreCase = false);
FFDLL std::string ReplaceCopy(const std::string& src, const std::string& find,
		const std::string& replace, bool ignoreCase = false);
FFDLL std::string& ReplaceAll(std::string& src, const std::string& find,
		const std::string& replace, bool ignoreCase = false);
FFDLL std::string ReplaceAllCopy(const std::string& src, const std::string& find,
		const std::string& replace, bool ignoreCase = false);

FFDLL int IndexOf(const std::string& src, const std::string& find, bool ignoreCase =
		false);

enum StringCompressType {
	WithEmptyString, RemoveEmptyString
};

class FFDLL IDelimiter {
public:
	IDelimiter(const std::string& delimiterStr);
	virtual ~IDelimiter();

	virtual int Find(const std::string& str) const = 0;
	virtual int DelimiterSize() const = 0;

protected:
	std::string m_delimiterStr;
};

class FFDLL IsAnyOf: public IDelimiter {
public:
	IsAnyOf(const std::string& delimiter);

	int Find(const std::string& str) const;
	int DelimiterSize() const;
};

class FFDLL IsStringOf: public IDelimiter {
public:
	IsStringOf(const std::string& delimiter);

	int Find(const std::string& str) const;
	int DelimiterSize() const;
};

FFDLL std::vector<std::string> Split(const std::string& text,
		const IDelimiter& delimiterChecker, StringCompressType compressType =
				WithEmptyString);

//十六进制字符转数字
FFDLL unsigned int HexAToI(char x);
FFDLL char IToHexA(unsigned int x);

class FFDLL String : public std::string{
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
