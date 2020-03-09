/*
 * UUID.h
 *
 *  Created on: Jan 22, 2020
 *      Author: liyawu
 */

#ifndef FF_UUID_H_
#define FF_UUID_H_

#include <ff/ff_config.h>
#include <string>
#include <ostream>

namespace NS_FF {

typedef uint8_t uuid_t[16];

class UUID {
public:
	UUID();
	UUID(const std::string& uuidStr);
	~UUID();

	const uuid_t* getBuffer() const;
	std::string toString() const;
	operator std::string() const;
private:
	uuid_t m_uuid;
};

std::ostream& operator<<(std::ostream& o, const UUID& u);

} /* namespace NS_FF */

#endif /* FF_UUID_H_ */