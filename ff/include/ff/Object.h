/*
 * Object.h
 *
 *  Created on: Oct 14, 2016
 *      Author: ducky
 */

#ifndef FF_OBJECT_H_
#define FF_OBJECT_H_

#include <ff/ff_config.h>
#include <string>

NS_FF_BEG

class LIBFF_API Object {
public:
	virtual std::string getClassName() const;
	virtual std::string getFullClassName() const;
	static unsigned long CppVeraion();

protected:
	Object();
	virtual ~Object();
};

NS_FF_END

#endif /* FF_OBJECT_H_ */
