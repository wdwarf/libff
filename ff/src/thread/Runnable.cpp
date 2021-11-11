/*
 * Runnable.cpp
 *
 *  Created on: Aug 7, 2019
 *      Author: root
 */

#include <ff/Runnable.h>

using namespace std;

NS_FF_BEG

class LIBFF_API _FuncRunnable : public Runnable {
 public:
  _FuncRunnable(RunnableFunc func) : m_func(func) {}

  void run() override {
    try {
      this->m_func();
    } catch (std::exception &e) {
      throw;
    }
  }

 private:
  RunnableFunc m_func;
};

LIBFF_API RunnablePtr MakeRunnable(RunnableFunc func) {
  return std::make_shared<_FuncRunnable>(func);
}

NS_FF_END
