/*
 * CliPacket.h
 *
 *  Created on: Mar 25, 2020
 *      Author: liyawu
 */

#ifndef CLI_CLIPACKET_H_
#define CLI_CLIPACKET_H_

#include <string>
#include <map>
#include <ff/ff_config.h>
#include <ff/Variant.h>

NS_FF_BEG

using CliMembers = std::map<std::string, Variant>;

class CliPacket {
public:
	CliPacket();
	virtual ~CliPacket();

	bool parse(const std::string &cmd);
	void reset();

	const std::string& getAction() const;
	const std::string& getObj() const;
	const CliMembers& getMembers() const;

private:
	std::string m_action;
	std::string m_obj;
	CliMembers m_members;
};

NS_FF_END

#endif /* CLI_CLIPACKET_H_ */
