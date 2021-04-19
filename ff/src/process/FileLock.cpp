/*
 * FileLock.cpp
 *
 *  Created on: Jan 21, 2020
 *      Author: liyawu
 */

#include <ff/FileLock.h>
#ifdef _WIN32

#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#endif

NS_FF_BEG

FileLock::FileLock(const std::string& file) {
#ifdef _WIN32
	this->m_fd = CreateFile(file.c_str(),                   // lpFileName
		GENERIC_WRITE | GENERIC_READ,               // dwDesiredAccess
		0,                          // dwShareMode
		NULL,                       // lpSecurityAttributes
		CREATE_ALWAYS,              // dwCreationDisposition
		FILE_ATTRIBUTE_NORMAL, // dwFlagsAndAttributes
		NULL);
#else
	this->m_fd = ::open(file.c_str(), O_RDONLY | O_CREAT, 0664);
#endif
}

FileLock::~FileLock() {
#ifdef _WIN32
	::CloseHandle(this->m_fd);
#else
	::close(this->m_fd);
#endif
}

void FileLock::lock() {
#ifdef _WIN32
	LockFileEx(this->m_fd, LOCKFILE_FAIL_IMMEDIATELY, (DWORD)0, (DWORD)200, (DWORD)0, nullptr);
#else
	::flock(this->m_fd, LOCK_EX);
#endif
}

bool FileLock::trylock() {
#ifdef _WIN32
	return false;
#else
	return (0 == ::flock(this->m_fd, LOCK_EX | LOCK_NB));
#endif
}

void FileLock::unlock() {
#ifdef _WIN32
	UnlockFileEx(this->m_fd, 0, 200, 0, nullptr);
#else
	::flock(this->m_fd, LOCK_UN);
#endif
}

NS_FF_END
