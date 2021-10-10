/*
 * Runnable.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/Runnable.h>

using namespace std;

NS_FF_BEG

LIBFF_API RunnablePtr MakeRunnable(RunnableFunc func) {
	return std::make_shared<_FuncRunnable>(func);
}

NS_FF_END
