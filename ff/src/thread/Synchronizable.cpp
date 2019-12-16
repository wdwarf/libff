/*
 * Synchronizable.cpp
 *
 *  Created on: Dec 16, 2019
 *      Author: root
 */

#include <ff/Synchronizable.h>

using namespace std;

namespace NS_FF {

Synchronizable::Synchronizable() {
}

Synchronizable::~Synchronizable() {
}

void Synchronizable::lock() {
	this->m_mutex.lock();
}

void Synchronizable::unlock() {
	this->m_mutex.unlock();
}

Synchronize::Synchronize(Synchronizable &sync) : m_sync(sync){
	this->m_sync.lock();
}

Synchronize::~Synchronize() {
	this->m_sync.unlock();
}

} /* namespace NS_FF */
