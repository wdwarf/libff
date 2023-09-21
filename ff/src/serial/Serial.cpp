/*
 * Serial.cpp
 *
 *  Created on: 2018年7月23日
 *      Author: liyawu
 */

#include <ff/Serial.h>

#include <cstring>

#ifdef _WIN32
#include <ff/windows/Registry.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#endif

using namespace std;

NS_FF_BEG

#ifdef _WIN32

Serial::Serial() : m_fd(INVALID_HANDLE_VALUE), m_readTimeout(-1) {
  memset(&this->m_commTimeouts, 0, sizeof(COMMTIMEOUTS));
  memset(&this->m_dcb, 0, sizeof(DCB));
  this->m_dcb.DCBlength = sizeof(DCB);
  this->m_dcb.fBinary = true;
  this->m_dcb.fParity = false;
  this->m_dcb.fDtrControl = false;

  this->m_commTimeouts.ReadIntervalTimeout = 150;
  this->m_commTimeouts.ReadTotalTimeoutMultiplier = 0;
  this->m_commTimeouts.ReadTotalTimeoutConstant = -1;
  this->m_commTimeouts.ReadTotalTimeoutConstant = this->m_readTimeout;
  this->m_commTimeouts.WriteTotalTimeoutConstant = 2000;
  this->m_commTimeouts.WriteTotalTimeoutMultiplier = 0;

  memset(&this->m_overLappedRead, 0, sizeof(OVERLAPPED));
  memset(&this->m_overLappedWrite, 0, sizeof(OVERLAPPED));
  this->m_overLappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  this->m_overLappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  this->setBaudrate(9600);
  this->setParity(Parity::NONE);
  this->setDatabit(8);
  this->setStopBit(StopBit::_1);
}

Serial::~Serial() {
  this->close();
  CloseHandle(this->m_overLappedRead.hEvent);
  CloseHandle(this->m_overLappedWrite.hEvent);
}

void Serial::open(const std::string& device) {
  if (this->isOpen()) {
    THROW_EXCEPTION(SerialException, device + " has opened.", 0);
  }

  _W(string) strPort = _T("\\\\.\\") + device;

  this->m_fd = CreateFile(strPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                          NULL, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

  if (INVALID_HANDLE_VALUE == this->m_fd)
    THROW_EXCEPTION(SerialException, device + " open failed." + strerror(errno),
                    errno);

  if (!SetupComm(this->m_fd, 1024, 1024) ||
      !SetCommTimeouts(this->m_fd, &this->m_commTimeouts) ||
      !SetCommState(this->m_fd, &this->m_dcb)) {
    this->close();
    THROW_EXCEPTION(SerialException, device + " open failed." + strerror(errno),
                    errno);
  }
  PurgeComm(this->m_fd, PURGE_TXCLEAR | PURGE_RXCLEAR);
  ClearCommError(this->m_fd, NULL, NULL);
}

bool Serial::isOpen() const { return (INVALID_HANDLE_VALUE != this->m_fd); }

void Serial::close() {
  if (!this->isOpen()) return;

  CloseHandle(this->m_fd);
  this->m_fd = INVALID_HANDLE_VALUE;
}

int Serial::read(char* buf, int len, int msTimeout) {
  if (!this->isOpen()) return -1;

  if (msTimeout != this->m_readTimeout) {
    this->m_readTimeout = msTimeout;
    this->m_commTimeouts.ReadTotalTimeoutConstant = this->m_readTimeout;
    SetCommTimeouts(this->m_fd, &this->m_commTimeouts);
  }

  DWORD readBytes = 0;
  DWORD evMask = 0;
  SetCommMask(this->m_fd, EV_RXCHAR);
  if (!WaitCommEvent(this->m_fd, &evMask, &this->m_overLappedRead)) {
    if (GetLastError() != ERROR_IO_PENDING) return -1;
    WaitForSingleObject(this->m_overLappedRead.hEvent, msTimeout);
    if (EV_RXCHAR == (EV_RXCHAR & evMask)) {
      if (!ReadFile(this->m_fd, buf, len, &readBytes, &m_overLappedRead)) {
        if (GetLastError() != ERROR_IO_PENDING) {
          CancelIoEx(this->m_fd, &m_overLappedRead);
          return -1;
        }
        if (!GetOverlappedResult(this->m_fd, &m_overLappedRead, &readBytes,
                                 TRUE))
          return -1;
      }
    }
  }

  return readBytes;
}

int Serial::send(const void* buf, int len) {
  if (!this->isOpen()) return -1;

  DWORD writeBytes = 0;
  const uint8_t* p = (const uint8_t*)buf;
  while (writeBytes < len) {
    DWORD sendBytes = 0;
    if (!WriteFile(this->m_fd, (p + writeBytes), len - writeBytes, &sendBytes,
                   &m_overLappedWrite)) {
      if (GetLastError() != ERROR_IO_PENDING) {
        CancelIoEx(this->m_fd, &m_overLappedWrite);
        return -1;
      }
      if (!GetOverlappedResult(this->m_fd, &m_overLappedWrite, &sendBytes,
                               TRUE))
        return -1;
    }
    writeBytes += sendBytes;
  }
  return writeBytes;
}

void Serial::flush() {
  //
}

int Serial::getBaudrate() const { return this->m_dcb.BaudRate; }

void Serial::setBaudrate(int baudrate) {
  /** 设置波特率 **/
  this->m_dcb.BaudRate = baudrate;
  if (this->isOpen()) SetCommState(this->m_fd, &this->m_dcb);
}

int Serial::getDatabit() const { return this->m_dcb.ByteSize; }

void Serial::setDatabit(int databit) {
  /** 设置数据位 **/
  this->m_dcb.ByteSize = databit;
  if (this->isOpen()) SetCommState(this->m_fd, &this->m_dcb);
}

Serial::Fd Serial::getFd() const { return this->m_fd; }

Parity Serial::getParity() const { return Parity(this->m_dcb.Parity); }

void Serial::setParity(Parity parity) {
  /** 设置校验位 **/
  this->m_dcb.Parity = (BYTE)parity;
  if (this->isOpen()) SetCommState(this->m_fd, &this->m_dcb);
}

StopBit Serial::getStopBit() const { return StopBit(this->m_dcb.StopBits); }

void Serial::setStopBit(StopBit stopBit) {
  /** 设置停止位 **/
  this->m_dcb.StopBits = (BYTE)stopBit;
  if (this->isOpen()) SetCommState(this->m_fd, &this->m_dcb);
}

std::list<std::string> Serial::ListComPorts() {
  std::list<std::string> re;
  Registry reg;
  reg.open(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM\\", KEY_READ);
  list<RegistryValue> valueList = reg.enumStringValues();
  for (auto& val : valueList) {
    re.push_back(val.asString());
  }
  return re;
}

#else

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

Serial::Serial() : m_fd(0) {
  //
}

Serial::~Serial() { this->close(); }

void Serial::open(const std::string &device) {
  if (this->isOpen()) {
    THROW_EXCEPTION(SerialException, device + " has opened.", 0);
  }

  this->m_fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (-1 == this->m_fd) {
    THROW_EXCEPTION(SerialException, device + " open failed." + strerror(errno),
                    errno);
  }

  tcgetattr(this->m_fd, &oldTermios);
  tcgetattr(this->m_fd, &newTermios);

  this->newTermios.c_cflag |= (CLOCAL | CREAD);  // | CRTSCTS;

  this->newTermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  this->newTermios.c_oflag &= ~OPOST;
  this->newTermios.c_oflag &= ~(ONLCR | OCRNL);

  this->newTermios.c_iflag &= ~(ICRNL | INLCR);
  this->newTermios.c_iflag &= ~(IXON | IXOFF | IXANY);

  this->newTermios.c_cc[VTIME] = 0; /* unit: 1/10 second. */
  this->newTermios.c_cc[VMIN] = 1;  /* minimal characters for reading */
  tcflush(this->m_fd, TCIFLUSH);
  tcsetattr(this->m_fd, TCSANOW, &this->newTermios);
}

bool Serial::isOpen() const { return (this->m_fd > 0); }

void Serial::close() {
  /* flush output data before close and restore old attribute */
  tcsetattr(this->m_fd, TCSADRAIN, &oldTermios);
  ::close(this->m_fd);
  this->m_fd = -1;
}

int Serial::read(char *buf, int len, int msTimeout) {
  fd_set fs;
  FD_ZERO(&fs);
  FD_SET(this->m_fd, &fs);
  timeval tv;
  timeval *pTv = NULL;
  if (msTimeout >= 0) {
    tv.tv_sec = msTimeout / 1000;
    tv.tv_usec = (msTimeout % 1000) * 1000;
    pTv = &tv;
  }
  int re = ::select(this->m_fd + 1, &fs, 0, 0, pTv);
  if (re > 0) {
    return ::read(this->m_fd, buf, len);
  }
  return re;
}

int Serial::send(const void *buf, int len) {
  fd_set fs;
  FD_ZERO(&fs);
  FD_SET(this->m_fd, &fs);
  timeval tv;
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  int re = ::select(this->m_fd + 1, 0, &fs, 0, &tv);
  if (re > 0) {
    return ::write(this->m_fd, buf, len);
  }
  return re;
}

void Serial::flush() { tcflush(this->m_fd, TCIOFLUSH); }

int Serial::getBaudrate() const {
  return _Baudrate(cfgetospeed(&this->newTermios));
}

void Serial::setBaudrate(int baudrate) {
  /** 设置波特率 **/
  unsigned int bd = Baudrate(baudrate);
  if (0 != cfsetispeed(&this->newTermios, bd) ||
      0 != cfsetospeed(&this->newTermios, bd)) {
    THROW_EXCEPTION(SerialException,
                    string("Baudrate set failed.") + strerror(errno), errno);
  }

  tcflush(this->m_fd, TCIFLUSH);
  if (0 != tcsetattr(this->m_fd, TCSANOW, &this->newTermios)) {
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

  tcflush(this->m_fd, TCIFLUSH);
  if (0 != tcsetattr(this->m_fd, TCSANOW, &this->newTermios)) {
    THROW_EXCEPTION(SerialException,
                    string("Databit set failed.") + strerror(errno), errno);
  }
}

int Serial::getFd() const { return this->m_fd; }

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

  tcflush(this->m_fd, TCIFLUSH);
  if (0 != tcsetattr(this->m_fd, TCSANOW, &this->newTermios)) {
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

  tcflush(this->m_fd, TCIFLUSH);
  if (0 != tcsetattr(this->m_fd, TCSANOW, &this->newTermios)) {
    THROW_EXCEPTION(SerialException,
                    string("StopBit set failed.") + strerror(errno), errno);
  }
}

std::list<std::string> Serial::ListComPorts() {
  return std::list<std::string>();
}

#endif

NS_FF_END
