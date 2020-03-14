/*
 * TestDef.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef TEST_TESTDEF_H_
#define TEST_TESTDEF_H_

#include <ff/ff_config.h>
#include <ff/Log.h>

#define TEST_LOG_MODULE "ff_test"

#define MKLOGFUNC(LV) NS_FF::Log(TEST_LOG_MODULE)(NS_FF::LogLevel::LV)\
	.setLineNumber(__LINE__).setFileName(__FILE__).setFunctionName(__func__)

#define LOGI MKLOGFUNC(Info)
#define LOGD MKLOGFUNC(Debug)
#define LOGE MKLOGFUNC(Error)

#endif /* TEST_TESTDEF_H_ */
