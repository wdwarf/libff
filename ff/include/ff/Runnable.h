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

namespace NS_FF {

EXCEPTION_DEF(RunnableException);

class FFDLL Runnable: virtual public Object {
public:
	Runnable() = default;
	virtual ~Runnable() = default;

	virtual void run() = 0;
};

typedef std::shared_ptr<Runnable> RunnablePtr;
typedef void (*FRunnableFunc)();
typedef std::function<void()> RunnableFunc;

FFDLL RunnablePtr MakeRunnable(FRunnableFunc func);
FFDLL RunnablePtr MakeRunnable(RunnableFunc func);

} /* namespace NS_FF */

#endif /* FF_RUNNABLE_H_ */
