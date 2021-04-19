/*
 * StringWrapper.h
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#ifndef FF_STRINGWRAPPER_H_
#define FF_STRINGWRAPPER_H_

#include <string>
#include <sstream>
#include <initializer_list>
#include <ff/ff_config.h>

NS_FF_BEG

class LIBFF_API StringWrapper: protected std::stringstream {
public:
	StringWrapper();
	virtual ~StringWrapper();

	template<class T> StringWrapper(const T& t) {
		std::stringstream& str = *this;
		str << t;
	}

	template <typename T, typename... TAIL>
	StringWrapper(const T& t, const TAIL&... tail){
		this->operator()(t);
		this->operator()(std::forward<TAIL>(tail)...);
	}

	template<class T> StringWrapper& operator()(const T& t) {
		std::stringstream& str = *this;
		str << t;
		return *this;
	}

	template <typename T, typename... TAIL>
	StringWrapper& operator()(const T& t, const TAIL&... tail) {
		this->operator()(t);
		this->operator()(std::forward<TAIL>(tail)...);
		return *this;
	}

	template<class T> StringWrapper& operator<<(const T& t) {
		return this->operator()(t);
	}

	std::string toString() const;
	operator std::string() const;

	friend std::ostream& operator<<(std::ostream& o, const StringWrapper& wp);
};

typedef StringWrapper SW;

NS_FF_END

#endif /* FF_STRINGWRAPPER_H_ */
