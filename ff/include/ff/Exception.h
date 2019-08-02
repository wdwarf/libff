/*
 * Exception.h
 *
 *  Created on: Sep 6, 2016
 *      Author: ducky
 */

#ifndef FF_EXCEPTION_H_
#define FF_EXCEPTION_H_

#include <ff/Object.h>
#include <exception>
#include <string>
#include <ostream>

namespace NS_FF {

#ifndef _GLIBCXX_USE_NOEXCEPT
#define _GLIBCXX_USE_NOEXCEPT throw()
#endif

#define _throws(x)

class Exception: public std::exception, public Object {
public:
	Exception(const std::string& msg, int errNo = 0, int lineNumber = 0,
			const std::string& functionName = "", const std::string& fileName =
					"") _GLIBCXX_USE_NOEXCEPT;
	virtual ~Exception() _GLIBCXX_USE_NOEXCEPT;

	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT;
	virtual int getErrNo() const;
	const std::string& getFileName() const;
	const std::string& getFunctionName() const;
	int getLineNumber() const;

	friend std::ostream& operator<<(std::ostream& o, const Exception& e);
private:
	int errNo;
	int lineNumber;
	std::string functionName;
	std::string fileName;
	std::string msg;
};

#define EXCEPTION_DEF(exceptionName) class exceptionName : public NS_FF::Exception{\
	public:\
	exceptionName(const std::string& msg, int errNo = 0, int lineNumber = 0, \
			const std::string& functionName = "", const std::string& fileName = "") _GLIBCXX_USE_NOEXCEPT \
			: Exception(msg, errNo, lineNumber, functionName, fileName){}\
	virtual ~exceptionName() _GLIBCXX_USE_NOEXCEPT{}\
};

#define EXCEPTION_DEF2(exceptionName, parentClass) class exceptionName : public parentClass{\
	public:\
	exceptionName(const std::string& msg, int errNo = 0, int lineNumber = 0, \
			const std::string& functionName = "", const std::string& fileName = "") _GLIBCXX_USE_NOEXCEPT \
			: parentClass(msg, errNo, lineNumber, functionName, fileName){}\
	virtual ~exceptionName() _GLIBCXX_USE_NOEXCEPT{}\
};

#define MK_EXCEPTION(exceptionName, msg, errNo) exceptionName(msg, errNo, __LINE__, __FUNCTION__, __FILE__)
#define THROW_EXCEPTION(exceptionName, msg, errNo) throw MK_EXCEPTION(exceptionName, msg, errNo)

} /* namespace NS_FF */

#endif /* FF_EXCEPTION_H_ */