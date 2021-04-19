/*
 * Synchronizable.cpp
 *
 *  Created on: Dec 16, 2019
 *      Author: root
 */

#include <ff/Synchronizable.h>

using namespace std;

NS_FF_BEG

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

bool Synchronizable::trylock(){
	return this->m_mutex.try_lock();
}

NS_FF_END
