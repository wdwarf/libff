/*
 * BlockingList.h
 *
 *  Created on: Sep 8, 2019
 *      Author: root
 */

#ifndef FF_BLOCKINGLIST_H_
#define FF_BLOCKINGLIST_H_

#include <list>
#include <mutex>
#include <condition_variable>
#include <ff/ff_config.h>
#include <ff/Noncopyable.h>

NS_FF_BEG

template<class T>
class BlockingList {
public:
	typedef std::list<T> ListT;

	BlockingList() {
	}
	~BlockingList() {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		m_list.clear();
	}

	BlockingList(const BlockingList& l) {
		std::lock_guard<std::mutex> lk(l.m_mutex);
		std::lock_guard<std::mutex> lk2(this->m_mutex);
		this->m_list = l.m_list;
	}

	BlockingList& operator=(const BlockingList& l) {
		std::lock_guard<std::mutex> lk(l.m_mutex);
		std::lock_guard<std::mutex> lk2(this->m_mutex);
		this->m_list = l.m_list;
		return *this;
	}

	BlockingList(BlockingList&& l) {
		std::lock_guard<std::mutex> lk(l.m_mutex);
		std::lock_guard<std::mutex> lk2(this->m_mutex);
		this->m_list = std::move(l.m_list);
	}

	BlockingList& operator=(BlockingList&& l) {
		std::lock_guard<std::mutex> lk(l.m_mutex);
		std::lock_guard<std::mutex> lk2(this->m_mutex);
		this->m_list = std::move(l.m_list);
		return *this;
	}

	typename ListT::size_type size() const{
		std::lock_guard<std::mutex> lk(this->m_mutex);
		return this->m_list.size();
	}

	bool empty() const{
		std::lock_guard<std::mutex> lk(this->m_mutex);
		return this->m_list.empty();
	}

	void clear() {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		this->m_list.clear();
	}

	void push_front(const T& t) {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_list.push_front(t);
		this->m_cond.notify_one();
	}

	void push_back(const T& t) {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_list.push_back(t);
		this->m_cond.notify_one();
	}

	T pop_front() {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_cond.wait(lk, [&] {return !this->m_list.empty();});
		T t = std::move(this->m_list.front());
		this->m_list.pop_front();
		return std::move(t);
	}

	T pop_back() {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_cond.wait(lk, [&] {return !this->m_list.empty();});
		T t = std::move(this->m_list.back());
		this->m_list.pop_back();
		return std::move(t);
	}

	bool pop_front(T& t, uint32_t timeoutMs) {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		if (!this->m_cond.wait_for(lk,
						std::chrono::milliseconds(timeoutMs),
						[&] {return !this->m_list.empty();}))
			return false;
		t = std::move(this->m_list.front());
		this->m_list.pop_front();
		return true;
	}

	bool pop_back(T& t, uint32_t timeoutMs) {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		if (!this->m_cond.wait_for(lk,
						std::chrono::milliseconds(timeoutMs),
						[&] {return !this->m_list.empty();}))
			return false;
		t = std::move(this->m_list.back());
		this->m_list.pop_back();
		return true;
	}

	void sort() {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_list.sort();
	}

	void unique() {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_list.unique();
	}

	void merge(ListT&& l) {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		this->m_list.merge(std::move(l));
		this->m_cond.notify_one();
	}

	void merge(ListT& l) {
		this->merge(std::move(l));
	}

	void merge(BlockingList&& l) {
		std::lock_guard<std::mutex> lk(l.m_mutex);
		this->merge(l.m_list);
	}

	void merge(BlockingList& l) {
		this->merge(std::move(l));
	}

	template<typename F>
	void for_each(F f) const{
		std::lock_guard<std::mutex> lk(this->m_mutex);
		for(auto& i : this->m_list){
			f(i);
		}
	}

	ListT list() const {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		return this->m_list;
	}

	void notify() {
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_cond.notify_one();
	}

private:
	ListT m_list;
	mutable std::mutex m_mutex;
	std::condition_variable m_cond;

	template<typename _Tp>
	friend inline bool operator==(const BlockingList<_Tp>& __x,
			const BlockingList<_Tp>& __y);

	template<typename _Tp>
	friend inline bool operator<(const BlockingList<_Tp>& __x,
			const BlockingList<_Tp>& __y);

	template<typename _Tp>
	friend inline bool operator!=(const BlockingList<_Tp>& __x,
			const BlockingList<_Tp>& __y);

	template<typename _Tp>
	friend inline bool operator>(const BlockingList<_Tp>& __x,
			const BlockingList<_Tp>& __y);

	template<typename _Tp>
	friend inline bool operator<=(const BlockingList<_Tp>& __x,
			const BlockingList<_Tp>& __y);

	template<typename _Tp, typename _Alloc>
	friend inline bool operator>=(const BlockingList<_Tp>& __x,
			const BlockingList<_Tp>& __y);
};

template<typename _Tp>
inline bool operator==(const BlockingList<_Tp>& __x,
		const BlockingList<_Tp>& __y) {
	std::lock_guard<std::mutex> lk(__x.m_mutex);
	std::lock_guard<std::mutex> lk2(__y.m_mutex);
	return __x.m_list == __y.m_list;
}

template<typename _Tp>
inline bool operator<(const BlockingList<_Tp>& __x,
		const BlockingList<_Tp>& __y) {
	std::lock_guard<std::mutex> lk(__x.m_mutex);
	std::lock_guard<std::mutex> lk2(__y.m_mutex);
	return __x.m_list < __y.m_list;
}

template<typename _Tp>
inline bool operator!=(const BlockingList<_Tp>& __x,
		const BlockingList<_Tp>& __y) {
	return !(__x == __y);
}

template<typename _Tp>
inline bool operator>(const BlockingList<_Tp>& __x,
		const BlockingList<_Tp>& __y) {
	std::lock_guard<std::mutex> lk(__x.m_mutex);
	std::lock_guard<std::mutex> lk2(__y.m_mutex);
	return __y.m_list < __x.m_list;
}

template<typename _Tp>
inline bool operator<=(const BlockingList<_Tp>& __x,
		const BlockingList<_Tp>& __y) {
	return !(__y < __x);
}

template<typename _Tp, typename _Alloc>
inline bool operator>=(const BlockingList<_Tp>& __x,
		const BlockingList<_Tp>& __y) {
	return !(__x < __y);
}

NS_FF_END

#endif /* FF_BLOCKINGLIST_H_ */
