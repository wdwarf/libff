/*
 * WCRC.h
 *
 *  Created on: Aug 2, 2012
 *      Author: ducky
 */

#ifndef FF_CRC_H_
#define FF_CRC_H_

#include <ff/Object.h>

namespace NS_FF {

class CRC: virtual public Object {
public:
	static unsigned short CRC16(const unsigned char* ptr, unsigned int len);

private:
	static unsigned short CRC16_1(const unsigned char *ptr, unsigned int len);
	static unsigned short CRC16_2(const unsigned char *ptr, unsigned int len);
	static unsigned short CRC16_3(const unsigned char *ptr, unsigned int len);

	CRC();
	virtual ~CRC();
};

} /* namespace NS_FF */

#endif /* FF_CRC_H_ */
