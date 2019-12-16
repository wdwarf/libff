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

namespace NS_FF {

class FFDLL StringWrapper: protected std::stringstream {
public:
	StringWrapper();
	virtual ~StringWrapper();

	template<class T> StringWrapper(const std::initializer_list<T>& args) {
		for (auto& arg : args)
			*((std::stringstream*) this) << arg;
	}

	template<class T> StringWrapper(const T& t) {
		std::stringstream& str = *this;
		str << t;
	}

	template<class T> StringWrapper& operator()(const T& t) {
		std::stringstream& str = *this;
		str << t;
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

} /* namespace NS_FF */

#endif /* FF_STRINGWRAPPER_H_ */
