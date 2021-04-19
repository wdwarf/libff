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

class LIBFF_API Runnable {
public:
	Runnable() = default;
	virtual ~Runnable() = default;

	virtual void run() = 0;
};

typedef std::shared_ptr<Runnable> RunnablePtr;

template<class T>
class _FuncRunnable: public Runnable {
public:
	_FuncRunnable(T func) :
			m_func(func) {
	}

	void run() override {
		try {
			this->m_func();
		} catch (std::exception &e) {
			throw;
		}
	}

private:
	T m_func;
};

template<class Func>
RunnablePtr MakeRunnable(Func func) {
	return std::make_shared<_FuncRunnable<Func> >(func);
}

NS_FF_END

#endif /* FF_RUNNABLE_H_ */
