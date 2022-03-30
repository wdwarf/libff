/*
 * Serial.h
 *
 *  Created on: 2018年7月23日
 *      Author: liyawu
 */

#ifndef FF_SERIAL_H_
#define FF_SERIAL_H_

#include <ff/Exception.h>
#include <ff/Noncopyable.h>
#include <ff/Object.h>
#include <ff/Synchronizable.h>
#include <ff/ff_config.h>

#include <list>
#include <memory>
#include <string>

#ifndef _WIN32
#include <termios.h>
#endif

NS_FF_BEG

EXCEPTION_DEF(SerialException);

enum class Parity {
  NONE,  //无校验
  ODD,   //奇校验
  EVEN   //偶校验
};

//
enum class StopBit {
  _1,    // 1
  _1P5,  // 1.5
  _2,    // 2
};

class LIBFF_API Serial : public Synchronizable, protected Noncopyable {
 public:
#ifdef _WIN32
  using Fd = HANDLE;
#else
  using Fd = int;
#endif

  Serial();
  virtual ~Serial();

  void open(const std::string &device);
  bool isOpen() const;
  void close();

  int read(char *buf, int len, int msTimeout = -1);
  int send(const void *buf, int len);

  int getBaudrate() const;
  void setBaudrate(int baudrate);
  int getDatabit() const;
  void setDatabit(int databit);
  Fd getFd() const;
  Parity getParity() const;
  void setParity(Parity parity);
  StopBit getStopBit() const;
  void setStopBit(StopBit stopBit);
  void flush();

  //列出系统所有的串口号
  static std::list<std::string> ListComPorts();

 private:
  Fd m_fd;
#ifndef _WIN32
  struct termios newTermios;
  struct termios oldTermios;
#else
  DWORD m_readTimeout;
  COMMTIMEOUTS m_commTimeouts;
  DCB m_dcb;
  OVERLAPPED m_overLappedRead;
  OVERLAPPED m_overLappedWrite;
#endif
};

using SerialPtr = std::shared_ptr<Serial>;

NS_FF_END

#endif /* FF_SERIAL_H_ */
