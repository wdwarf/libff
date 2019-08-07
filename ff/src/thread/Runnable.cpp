/*
 * Runnable.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/Runnable.h>

using namespace std;

namespace NS_FF {

class FuncRunnable: public Runnable {
public:
	FuncRunnable(RunnableFunc func) :
			m_func(func) {
	}

	void run() override {
		try {
			this->m_func();
		} catch (std::exception& e) {
			throw;
		}
	}

private:
	RunnableFunc m_func;
};

RunnablePtr MakeRunnable(FRunnableFunc func) {
	return MakeRunnable(bind(func));
}

RunnablePtr MakeRunnable(RunnableFunc func) {
	return std::make_shared<FuncRunnable>(func);
}

} /* namespace NS_FF */
