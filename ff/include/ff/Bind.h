/*
 * Bind.h
 *
 *  Created on: Jan 6, 2020
 *      Author: root
 */

#ifndef FF_BIND_H_
#define FF_BIND_H_

#include <ff/ff_config.h>
#include <functional>

NS_FF_BEG

#if _MSC_VER
#define __placeholder std::_Ph
#else
#define __placeholder std::_Placeholder
#endif

template<int N, int ...I>
struct placeholader_trais: placeholader_trais<N - 1, N - 1, I...> {};

template<int ...I>
struct placeholader_trais<1, I...> {
	template<typename R, typename ...Args>
	static auto bind(R (*F)(Args...)) -> decltype(std::bind(F, __placeholder<I> {}...)) {
		return std::bind(F, __placeholder<I> {}...);
	}

	template<typename T, typename R, typename ...Args>
	static auto bind(T* obj, R (T::*F)(Args...)) -> decltype(std::bind(F, obj, __placeholder<I> {}...)) {
		return std::bind(F, obj, __placeholder<I> {}...);
	}
};

template<typename R, typename ...Args>
auto Bind(R (*f)(Args...)) -> decltype(placeholader_trais<sizeof...(Args)+1>::bind(f)) {
	return placeholader_trais<sizeof...(Args)+1>::bind(f);
}

template<typename T, typename R, typename ...Args>
auto Bind(T* t, R (T::*f)(Args...)) -> decltype(placeholader_trais<sizeof...(Args)+1>::bind(t, f)) {
	return placeholader_trais<sizeof...(Args)+1>::bind(t, f);
}

template<typename T, typename R, typename ...Args>
auto Bind(R (T::*f)(Args...), T* t) -> decltype(placeholader_trais<sizeof...(Args)+1>::bind(t, f)) {
	return placeholader_trais<sizeof...(Args)+1>::bind(t, f);
}

}
/* namespace NS_FF */

#endif /* FF_BIND_H_ */
