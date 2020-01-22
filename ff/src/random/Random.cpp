/*
 * Random.cpp
 *
 *  Created on: Jan 22, 2020
 *      Author: liyawu
 */

#include <ff/Random.h>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

namespace NS_FF {

namespace {

inline int openRandomFd() {
	int i = 0;
	int fd = 0;
	fd = open("/dev/urandom", O_RDONLY);
	if (fd == -1)
		fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
	if (fd >= 0) {
		i = fcntl(fd, F_GETFD);
		if (i >= 0)
			fcntl(fd, F_SETFD, i | FD_CLOEXEC);
	}

	struct timeval tv;
	gettimeofday(&tv, 0);
	srand((getpid() << 16) ^ getuid() ^ tv.tv_sec ^ tv.tv_usec);
	for (i = (tv.tv_sec ^ tv.tv_usec) & 0x1F; i > 0; i--)
		rand();
	return fd;
}

}

Random::Random() {
	this->m_fd = openRandomFd();
}

Random::~Random() {
	if (this->m_fd >= 0)
		::close(this->m_fd);
}

void Random::getRandomBytes(void *buf, uint32_t size) {
	uint32_t i = 0;
	ssize_t n = size;
	int failedCnt = 0;
	unsigned char *p = (unsigned char *) buf;

	if (this->m_fd >= 0) {
		while (n > 0) {
			ssize_t x = read(this->m_fd, p, n);
			if (x <= 0) {
				if (failedCnt++ > 16)
					break;
				continue;
			}
			n -= x;
			p += x;
			failedCnt = 0;
		}

	}

	for (p = (unsigned char *) buf, i = 0; i < size; i++)
		*p++ ^= (rand() >> 7) & 0xFF;

}

} /* namespace NS_FF */
