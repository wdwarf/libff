/*
 * TestDef.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef TEST_TESTDEF_H_
#define TEST_TESTDEF_H_

#include <ff/Log.h>

#define TEST_LOG_MODULE "ff_test"

#define MKLOGFUNC(LV) NS_FF::Log(TEST_LOG_MODULE)(NS_FF::LogLevel::LV)\
	.setLineNumber(__LINE__).setFileName(__FILE__).setFunctionName(__func__)

#define LDBG MKLOGFUNC(DEBUG)
#define LERR MKLOGFUNC(ERROR)

#endif /* TEST_TESTDEF_H_ */
