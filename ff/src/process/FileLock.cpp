/*
 * FileLock.cpp
 *
 *  Created on: Jan 21, 2020
 *      Author: u16
 */

#include <ff/FileLock.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

namespace NS_FF {

FileLock::FileLock(const std::string& file) {
	this->m_fd = ::open(file.c_str(), O_RDONLY | O_CREAT, 0664);
}

FileLock::~FileLock() {
	::close(this->m_fd);
}

void FileLock::lock() {
	::flock(this->m_fd, LOCK_EX);
}

bool FileLock::trylock() {
	return (0 == ::flock(this->m_fd, LOCK_EX | LOCK_NB));
}

void FileLock::unlock() {
	::flock(this->m_fd, LOCK_UN);
}

} /* namespace NS_FF */
