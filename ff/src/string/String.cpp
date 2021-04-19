/*
 * String.cpp
 *
 *  Created on: Sep 5, 2016
 *      Author: ducky
 */

#include <ff/String.h>
#include <string>
#include <cstring>
#include <sstream>
#include <functional>
#include <algorithm>
#include <regex>
#include <ff/Exception.h>

using namespace std;

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp		_strcmpi
#endif

NS_FF_BEG

inline int _isspace_(int c) {
	int re = isspace(c);
	if (0 == re) {
		re = ('\0' == c);
	}
	return re;
}

string& ToLower(string& str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

string ToLowerCopy(const string& str) {
	string re = str;
	ToLower(re);
	return re;
}

string& ToUpper(string& str) {
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

string ToUpperCopy(const string& str) {
	string re = str;
	ToUpper(re);
	return re;
}

string& TrimLeft(string& str) {
	string::iterator p = find_if(str.begin(), str.end(),
			not1(ptr_fun<int, int>(_isspace_)));
	str.erase(str.begin(), p);
	return str;
}

string TrimLeftCopy(const string& str) {
	string newStr = str;
	return TrimLeft(newStr);
}

string& TrimRight(string& str) {
	string::reverse_iterator p = find_if(str.rbegin(), str.rend(),
			not1(ptr_fun<int, int>(_isspace_)));
	str.erase(p.base(), str.end());
	return str;
}

string TrimRightCopy(const string& str) {
	string newStr = str;
	return TrimRight(newStr);
}

string& Trim(string& str) {
	return TrimRight(TrimLeft(str));
}

string TrimCopy(const string& str) {
	string newStr = str;
	return Trim(newStr);
}


string& Reverse(std::string& str){
	reverse(str.begin(), str.end());
	return str;
}

string ReverseCopy(const std::string& str){
	string newStr = str;
	return Reverse(newStr);
}

string& Replace(string& src, const string& find, const string& replace,
		bool ignoreCase) {
	string::size_type pos = string::npos;
	if (ignoreCase) {
		string s = src;
		string f = find;
		ToLower(s);
		ToLower(f);
		pos = s.find(f);
	} else {
		pos = src.find(find);
	}

	if (pos != string::npos) {
		src.replace(pos, find.length(), replace);
	}

	return src;
}

string ReplaceCopy(const string& src, const string& find, const string& replace,
		bool ignoreCase) {
	string str = src;
	return Replace(str, find, replace, ignoreCase);
}

string& ReplaceAll(string& src, const string& find, const string& replace,
		bool ignoreCase) {
	string::size_type pos = 0;
	string s = src;
	string f = find;
	if (ignoreCase) {
		ToLower(s);
		ToLower(f);
	}

	do {
		pos = s.find(f, pos);

		if (pos != string::npos) {
			src.replace(pos, find.length(), replace);
			s.replace(pos, find.length(), replace);
		} else {
			break;
		}
		pos += replace.length();
	} while (true);
	return src;
}

string ReplaceAllCopy(const string& src, const string& find,
		const string& replace, bool ignoreCase) {
	string str = src;
	return ReplaceAll(str, find, replace, ignoreCase);
}

int IndexOf(const string& src, const string& find, bool ignoreCase) {
	int re = -1;
	string::size_type pos = string::npos;
	if (ignoreCase) {
		string s = src;
		string f = find;
		ToLower(s);
		ToLower(f);
		pos = s.find(f);
	} else {
		pos = src.find(find);
	}

	if (pos != string::npos) {
		re = pos;
	}

	return re;
}

bool EndsWith(const std::string &src, const std::string &find, bool ignoreCase)
{
	if(src.length() < find.length()) return false;
	string s = src.substr(src.length() - find.length());
	if(ignoreCase){
		return ToLowerCopy(s) == ToLowerCopy(find);
	}

	return s == find;
}

bool StartsWith(const std::string &src, const std::string &find, bool ignoreCase)
{
	if(src.length() < find.length()) return false;
	string s = src.substr(0, find.length());
	if(ignoreCase){
		return ToLowerCopy(s) == ToLowerCopy(find);
	}

	return s == find;
}

bool Match(const std::string& str, const std::string& reg){
	return regex_match(str, regex(reg));
}

unsigned int HexAToI(char x) {
	if ((x >= 48) && (x <= 57))
		x -= 48; /* 0-9 */
	else if ((x >= 97) && (x <= 102))
		x -= 87; /* a-f */
	else if ((x >= 65) && (x <= 70))
		x -= 55; /* A-F */
	else {
		stringstream str;
		str << "Invalid hex character[" << x << "]";
		THROW_EXCEPTION(Exception, str.str(), x);
	}
	return x;
}

char IToHexA(unsigned int x) {
	if (x >= 0 && x <= 9) {
		x += 48;
	}
	if (x >= 10 && x <= 15) {
		x += 55;
	} else {
		stringstream str;
		str << "Invalid hex number[" << x << "]";
		THROW_EXCEPTION(Exception, str.str(), x);
	}

	return x;
}

/////////////////////////////////////////////////////////////////
/// Split
/////////////////////////////////////////////////////////////////

IDelimiter::IDelimiter(const std::string& delimiterStr) :
		m_delimiterStr(delimiterStr) {
}

IDelimiter::~IDelimiter() {
}

IsAnyOf::IsAnyOf(const std::string& delimiter) :
		IDelimiter(delimiter) {

}

int IsAnyOf::Find(const std::string& str) const {
	for (string::size_type i = 0; i < str.length(); ++i) {
		if (string::npos != this->m_delimiterStr.find(str[i])) {
			return i;
		}
	}
	return -1;
}

int IsAnyOf::DelimiterSize() const {
	return 1;
}

IsStringOf::IsStringOf(const std::string& delimiter) :
		IDelimiter(delimiter) {

}

int IsStringOf::Find(const std::string& str) const {
	string::size_type pos = str.find(this->m_delimiterStr);
	if (string::npos != pos) {
		return pos;
	}

	return -1;
}

int IsStringOf::DelimiterSize() const {
	return this->m_delimiterStr.length();
}

std::vector<std::string> Split(const std::string& text,
		const IDelimiter& delimiterChecker, StringCompressType compressType) {
	std::vector<std::string> texts;
	std::string srcText = text;

	while (!srcText.empty()) {
		int pos = delimiterChecker.Find(srcText);
		if (pos >= 0) {
			auto findStr = srcText.substr(0, pos);
			srcText = srcText.substr(pos + delimiterChecker.DelimiterSize());

			if (WithEmptyString == compressType) {
				texts.push_back(findStr);
				continue;
			}

			Trim(findStr);
			if (!findStr.empty()) {
				texts.push_back(findStr);
			}
			continue;
		}

		texts.push_back(srcText);
		break;
	}
	return texts;
}

/////////////////////////////////////////////////////////////////
/// class String
/////////////////////////////////////////////////////////////////

String::String(const char* str, size_t len) : string(str, len){

}

bool String::equals(const String& str, bool isEgnoreCase) const
{
	return (isEgnoreCase ? 0 == strcasecmp(this->c_str(), str.c_str())
			: 0 == strcmp(this->c_str(), str.c_str()));
}

bool String::match(const String& regStr) const{
	return Match(*this, regStr);
}

String String::trimLeft() const {
	return TrimLeftCopy(*this);
}

String String::trimRight() const {
	return TrimRightCopy(*this);
}

String String::trim() const {
	return TrimCopy(*this);
}

String String::toLower() const {
	return ToLowerCopy(*this);
}

String String::toUpper() const {
	return ToUpperCopy(*this);
}

String String::reverse() const{
	return ReverseCopy(*this);
}

String String::replace(const String &find, const String &replace,
		bool ignoreCase) const {
	return ReplaceCopy(*this, find, replace, ignoreCase);
}

String String::replaceAll(const String &find, const String &replace,
		bool ignoreCase) const {
	return ReplaceAllCopy(*this, find, replace, ignoreCase);
}

int String::indexOf(const String &find, bool ignoreCase) const {
	return IndexOf(*this, find, ignoreCase);
}

bool String::endsWith(const String &find, bool ignoreCase) const {
	return EndsWith(*this, find, ignoreCase);
}

bool String::startsWith(const String &find, bool ignoreCase) const {
	return StartsWith(*this, find, ignoreCase);
}

std::vector<String> String::split(const IDelimiter &delimiterChecker,
		StringCompressType compressType) {
	std::vector<String> texts;
	std::string srcText = *this;

	while (!srcText.empty()) {
		int pos = delimiterChecker.Find(srcText);
		if (pos >= 0) {
			auto findStr = srcText.substr(0, pos);
			srcText = srcText.substr(pos + delimiterChecker.DelimiterSize());

			if (WithEmptyString == compressType) {
				texts.push_back(findStr);
				continue;
			}

			Trim(findStr);
			if (!findStr.empty()) {
				texts.push_back(findStr);
			}
			continue;
		}

		texts.push_back(srcText);
		break;
	}
	return texts;
}

NS_FF_END

