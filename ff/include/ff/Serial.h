/*
 * Serial.h
 *
 *  Created on: 2018年7月23日
 *      Author: liyawu
 */

#ifndef FF_SERIAL_H_
#define FF_SERIAL_H_

#include <ff/ff_config.h>
#include <ff/Object.h>
#include <ff/Noncopyable.h>
#include <ff/Exception.h>
#include <string>
#include <memory>
#include <termios.h>

namespace NS_FF {

EXCEPTION_DEF(SerialException);

enum class Parity {
	NONE,		//无校验
	ODD,		//奇校验
	EVEN		//偶校验
};

//
enum class StopBit {
	_1, 		//1
	_1P5, 		//1.5
	_2,			//2
};

class FFDLL Serial: protected Noncopyable {
public:
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
	int getFd() const;
	Parity getParity() const;
	void setParity(Parity parity);
	StopBit getStopBit() const;
	void setStopBit(StopBit stopBit);
	void flush();

private:
	int fd;
	struct termios newTermios;
	struct termios oldTermios;
};

std::shared_ptr<Serial> SerialPtr;

} /* namespace NS_FF */

#endif /* FF_SERIAL_H_ */
