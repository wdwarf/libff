/*
 * Runnable.h
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#ifndef FF_RUNNABLE_H_
#define FF_RUNNABLE_H_

#include <ff/Object.h>
#include <memory>
#include <functional>
#include <ff/Exception.h>

NS_FF_BEG

EXCEPTION_DEF(RunnableException);

using RunnableFunc = std::function<void ()>;

class LIBFF_API Runnable {
public:
	Runnable() = default;
	virtual ~Runnable() = default;

	virtual void run() = 0;
};

typedef std::shared_ptr<Runnable> RunnablePtr;

LIBFF_API RunnablePtr MakeRunnable(RunnableFunc func);

NS_FF_END

#endif /* FF_RUNNABLE_H_ */
