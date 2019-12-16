/*
 * IP.h
 *
 *  Created on: 2018年5月2日
 *      Author: liyawu
 */

#ifndef FF_IP_H_
#define FF_IP_H_

#include <ff/Object.h>
#include <ff/Exception.h>
#include <ff/Buffer.h>
#include <string>
#include <ostream>

namespace NS_FF {

class FFDLL IP: public Object {
public:
	IP();
	IP(const std::string& ip);
	virtual ~IP();

	enum class VersionType {
		UNKNOWN, V4, V6
	};

	void parse(const std::string& ip);
	bool isValid() const;
	std::string toString() const;
	operator std::string() const;
	VersionType getVersion() const;
	bool isV4() const;
	bool isV6() const;
	bool hasV6Scope() const;
	unsigned int getV6Scope() const;
	void clear();

	Buffer toBuffer() const;


	friend std::ostream& operator<<(std::ostream& o, const IP& ip);

private:
	static const unsigned int IPV6_LENGTH_NO_SCOPE = 16;
	static const unsigned int IPV6_LENGTH_WITH_SCOPE = 20;
	static const unsigned int BUFSIZE = 32;

	VersionType version;
	unsigned char addrBuffer[BUFSIZE];

	bool parseV4(const std::string& ip);
	bool parseV6(const std::string& ip);
};

} /* namespace NS_FF */

#endif /* FF_IP_H_ */

