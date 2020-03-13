/*
 * Serial.cpp
 *
 *  Created on: 2018年7月23日
 *      Author: liyawu
 */

#ifndef _WIN32

#include <ff/Serial.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <termios.h>

using namespace std;

namespace NS_FF {

static int Baudrate(int baudrate) {
	switch (baudrate) {
#ifdef B0
	case 0:
		return (B0);
#endif

#ifdef B50
	case 50:
		return (B50);
#endif

#ifdef B75
	case 75:
		return (B75);
#endif

#ifdef B110
	case 110:
		return (B110);
#endif

#ifdef B134
	case 134:
		return (B134);
#endif

#ifdef B150
	case 150:
		return (B150);
#endif

#ifdef B200
	case 200:
		return (B200);
#endif

#ifdef B300
	case 300:
		return (B300);
#endif

#ifdef B600
	case 600:
		return (B600);
#endif

#ifdef B128000
		case 1200:
		return (B1200);
#endif

#ifdef B2400
	case 2400:
		return (B2400);
#endif

#ifdef B9600
	case 9600:
		return (B9600);
#endif

#ifdef B19200
	case 19200:
		return (B19200);
#endif

#ifdef B38400
	case 38400:
		return (B38400);
#endif

#ifdef B57600
	case 57600:
		return (B57600);
#endif

#ifdef B115200
	case 115200:
		return (B115200);
#endif

#ifdef B128000
		case 128000:
		return (B128000);
#endif

#ifdef B230400
	case 230400:
		return (B230400);
#endif

#ifdef B256000
		case 256000:
		return (B256000);
#endif

#ifdef B460800
	case 460800:
		return (B460800);
#endif

#ifdef B500000
	case 500000:
		return (B500000);
#endif

#ifdef B576000
	case 576000:
		return (B576000);
#endif

#ifdef B921600
	case 921600:
		return (B921600);
#endif

#ifdef B1000000
	case 1000000:
		return (B1000000);
#endif

#ifdef B1152000
	case 1152000:
		return (B1152000);
#endif

#ifdef B1500000
	case 1500000:
		return (B1500000);
#endif

#ifdef B2000000
	case 2000000:
		return (B2000000);
#endif

#ifdef B2500000
	case 2500000:
		return (B2500000);
#endif

#ifdef B3000000
	case 3000000:
		return (B3000000);
#endif
	}

	THROW_EXCEPTION(SerialException,
			"Unsuppotr baudrate value: " + to_string(baudrate), baudrate);
}

static int _Baudrate(int baudrate) {
	switch (baudrate) {
#ifdef B0
	case B0:
		return (0);
#endif

#ifdef B50
	case B50:
		return (50);
#endif

#ifdef B75
	case B75:
		return (75);
#endif

#ifdef B110
	case B110:
		return (110);
#endif

#ifdef B134
	case B134:
		return (134);
#endif

#ifdef B150
	case B150:
		return (150);
#endif

#ifdef B200
	case B200:
		return (200);
#endif

#ifdef B300
	case B300:
		return (300);
#endif

#ifdef B600
	case B600:
		return (600);
#endif

#ifdef B128000
		case B1200:
		return (1200);
#endif

#ifdef B2400
	case B2400:
		return (2400);
#endif

#ifdef B9600
	case B9600:
		return (9600);
#endif

#ifdef B19200
	case B19200:
		return (19200);
#endif

#ifdef B38400
	case B38400:
		return (38400);
#endif

#ifdef B57600
	case B57600:
		return (57600);
#endif

#ifdef B115200
	case B115200:
		return (115200);
#endif

#ifdef B128000
		case B128000:
		return (128000);
#endif

#ifdef B230400
	case B230400:
		return (230400);
#endif

#ifdef B256000
		case B256000:
		return (256000);
#endif

#ifdef B460800
	case B460800:
		return (460800);
#endif

#ifdef B500000
	case B500000:
		return (500000);
#endif

#ifdef B576000
	case B576000:
		return (576000);
#endif

#ifdef B921600
	case B921600:
		return (921600);
#endif

#ifdef B1000000
	case B1000000:
		return (1000000);
#endif

#ifdef B1152000
	case B1152000:
		return (1152000);
#endif

#ifdef B1500000
	case B1500000:
		return (1500000);
#endif

#ifdef B2000000
	case B2000000:
		return (2000000);
#endif

#ifdef B2500000
	case B2500000:
		return (2500000);
#endif

#ifdef B3000000
	case B3000000:
		return (3000000);
#endif
	}

	return 0;
}

Serial::Serial() :
		fd(0) {
	//
}

Serial::~Serial() {
	this->close();
}

void Serial::open(const std::string &device) {
	if (this->isOpen()) {
		THROW_EXCEPTION(SerialException, device + " has opened.", 0);
	}

	this->fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (-1 == this->fd) {
		THROW_EXCEPTION(SerialException,
				device + " open failed." + strerror(errno), errno);
	}

	tcgetattr(this->fd, &oldTermios);
	tcgetattr(this->fd, &newTermios);

	this->newTermios.c_cflag |= (CLOCAL | CREAD); // | CRTSCTS;

	this->newTermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	this->newTermios.c_oflag &= ~OPOST;
	this->newTermios.c_oflag &= ~(ONLCR | OCRNL);

	this->newTermios.c_iflag &= ~(ICRNL | INLCR);
	this->newTermios.c_iflag &= ~(IXON | IXOFF | IXANY);

	this->newTermios.c_cc[VTIME] = 0; /* unit: 1/10 second. */
	this->newTermios.c_cc[VMIN] = 1; /* minimal characters for reading */
	tcflush(this->fd, TCIFLUSH);
	tcsetattr(this->fd, TCSANOW, &this->newTermios);
}

bool Serial::isOpen() const {
	return (this->fd > 0);
}

void Serial::close() {
	/* flush output data before close and restore old attribute */
	tcsetattr(this->fd, TCSADRAIN, &oldTermios);
	::close(this->fd);
	this->fd = -1;
}

int Serial::read(char *buf, int len, int msTimeout) {
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(this->fd, &fs);
	timeval tv;
	timeval *pTv = NULL;
	if (msTimeout >= 0) {
		tv.tv_sec = msTimeout / 1000;
		tv.tv_usec = (msTimeout % 1000) * 1000;
		pTv = &tv;
	}
	int re = ::select(this->fd + 1, &fs, 0, 0, pTv);
	if (re > 0) {
		return ::read(this->fd, buf, len);
	}
	return re;
}

int Serial::send(const void *buf, int len) {
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(this->fd, &fs);
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	int re = ::select(this->fd + 1, 0, &fs, 0, &tv);
	if (re > 0) {
		return ::write(this->fd, buf, len);
	}
	return re;
}

void Serial::flush() {
	tcflush(this->fd, TCIOFLUSH);
}

int Serial::getBaudrate() const {
	return _Baudrate(cfgetospeed(&this->newTermios));
}

void Serial::setBaudrate(int baudrate) {
	/** 设置波特率 **/
	unsigned int bd = Baudrate(baudrate);
	if (0 != cfsetispeed(&this->newTermios, bd)
			|| 0 != cfsetospeed(&this->newTermios, bd)) {
		THROW_EXCEPTION(SerialException,
				string("Baudrate set failed.") + strerror(errno), errno);
	}

	tcflush(this->fd, TCIFLUSH);
	if (0 != tcsetattr(this->fd, TCSANOW, &this->newTermios)) {
		THROW_EXCEPTION(SerialException,
				string("Baudrate set failed.") + strerror(errno), errno);
	}
}

int Serial::getDatabit() const {
	unsigned int size = this->newTermios.c_cflag & CSIZE;
	switch (size) {
	case CS8:
		return 8;
	case CS7:
		return 7;
	case CS6:
		return 6;
	case CS5:
		return 5;
	}
	return 0;
}

void Serial::setDatabit(int databit) {
	/** 设置数据位 **/
	this->newTermios.c_cflag &= ~CSIZE;
	switch (databit) {
	case 8:
		this->newTermios.c_cflag |= CS8;
		break;
	case 7:
		this->newTermios.c_cflag |= CS7;
		break;
	case 6:
		this->newTermios.c_cflag |= CS6;
		break;
	case 5:
		this->newTermios.c_cflag |= CS5;
		break;
	default:
		THROW_EXCEPTION(SerialException, "Invalid data bit.", databit);
	}

	tcflush(this->fd, TCIFLUSH);
	if (0 != tcsetattr(this->fd, TCSANOW, &this->newTermios)) {
		THROW_EXCEPTION(SerialException,
				string("Databit set failed.") + strerror(errno), errno);
	}
}

int Serial::getFd() const {
	return fd;
}

Parity Serial::getParity() const {
	if (PARENB & this->newTermios.c_cflag) {
		if (PARODD & this->newTermios.c_cflag) {
			return Parity::ODD;
		}
		return Parity::EVEN;
	} else {
		return Parity::NONE;
	}
}

void Serial::setParity(Parity parity) {
	/** 设置校验位 **/
	switch (parity) {
	case Parity::NONE:
		this->newTermios.c_cflag &= ~PARENB;
		this->newTermios.c_iflag &= ~INPCK;
		break;
	case Parity::ODD:
		this->newTermios.c_cflag |= PARENB;
		this->newTermios.c_iflag |= (INPCK | ISTRIP);
		this->newTermios.c_cflag |= PARODD;
		break;
	case Parity::EVEN:
		this->newTermios.c_cflag |= PARENB;
		this->newTermios.c_iflag |= (INPCK | ISTRIP);
		this->newTermios.c_cflag &= ~PARODD;
		break;
	default:
		THROW_EXCEPTION(SerialException, "Invalid parity.", int(parity));
	}

	tcflush(this->fd, TCIFLUSH);
	if (0 != tcsetattr(this->fd, TCSANOW, &this->newTermios)) {
		THROW_EXCEPTION(SerialException,
				string("Parity set failed.") + strerror(errno), errno);
	}
}

StopBit Serial::getStopBit() const {
	if (CSTOPB & this->newTermios.c_cflag) {
		return StopBit::_2;
	}
	return StopBit::_1;
}

void Serial::setStopBit(StopBit stopBit) {
	/** 设置停止位 **/
	switch (stopBit) {
	case StopBit::_1:
	case StopBit::_1P5:
		this->newTermios.c_cflag &= ~CSTOPB;
		break;
	case StopBit::_2:
		this->newTermios.c_cflag |= CSTOPB;
		break;
	default:
		THROW_EXCEPTION(SerialException, "Invalid stop bit.", int(stopBit));
	}

	tcflush(this->fd, TCIFLUSH);
	if (0 != tcsetattr(this->fd, TCSANOW, &this->newTermios)) {
		THROW_EXCEPTION(SerialException,
				string("StopBit set failed.") + strerror(errno), errno);
	}
}

} /* namespace NS_FF */

#endif
