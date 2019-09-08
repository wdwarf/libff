/*
 * BlockingList.h
 *
 *  Created on: Sep 8, 2019
 *      Author: root
 */

#ifndef FF_BLOCKINGLIST_H_
#define FF_BLOCKINGLIST_H_

#include <ff/ff_config.h>
#include <list>
#include <mutex>
#include <condition_variable>

namespace NS_FF {

template<class T>
class BlockingList {
public:
	typedef std::list<T> ListT;

	BlockingList(){}
	~BlockingList() {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		m_list.clear();
	}

	typename ListT::size_type size() {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		return this->m_list.size();
	}

	bool empty() {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		return this->m_list.empty();
	}

	bool clear() {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		return this->m_list.clear();
	}

	void pushFront(const T& t){
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_list.push_front(t);
		this->m_cond.notify_one();
	}

	void pushBack(const T& t){
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_list.push_back(t);
		this->m_cond.notify_one();
	}

	T popFront(){
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_cond.wait(lk, [&]{ return !this->m_list.empty(); });
		T t = std::move(this->m_list.front());
		this->m_list.pop_front();
		return std::move(t);
	}

	T popBack(){
		std::unique_lock<std::mutex> lk(this->m_mutex);
		this->m_cond.wait(lk, [&]{ return !this->m_list.empty(); });
		T t = std::move(this->m_list.back());
		this->m_list.pop_back();
		return std::move(t);
	}

private:
	ListT m_list;
	std::mutex m_mutex;
	std::condition_variable m_cond;
};

} /* namespace NS_FF */

#endif /* FF_BLOCKINGLIST_H_ */
