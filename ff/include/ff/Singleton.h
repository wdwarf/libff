/*
 * Singleton.h
 *
 *  Created on: Oct 15, 2016
 *      Author: ducky
 */

#ifndef FF_SINGLETON_H_
#define FF_SINGLETON_H_

#include <ff/Object.h>

namespace NS_FF {

/*
 * 这个单例会在第一次调用时初始化
 * */
template<class T>
class Singleton: public T, public Object {
public:
	static T* getInstance() {
		static Singleton<T> instance;
		return &instance;
	}

private:
	~Singleton() {
	}
	Singleton() {
	}

	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

};

/*
 * 这个单例会在main函数执行之前初始化
 * */
template<class T>
class Singleton2: public T, public Object {
private:
	struct object_creator {
		object_creator() {
			Singleton2<T>::getInstance();
		}
		inline void doNothing() const {
		}
	};
	static object_creator creator_object;

public:
	static T* getInstance() {
		static Singleton2<T> instance;
		creator_object.doNothing();
		return &instance;
	}

private:
	~Singleton2() {
	}
	Singleton2() {
	}

	Singleton2(const Singleton2&);
	Singleton2& operator=(const Singleton2&);

};

template<class T>
typename Singleton2<T>::object_creator Singleton2<T>::creator_object;

} /* namespace NS_FF */

#endif /* FF_SINGLETON_H_ */
