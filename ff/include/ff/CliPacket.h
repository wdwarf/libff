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

class CliResult{
public:
	CliResult();
	CliResult(uint32_t code, const std::string& data);
	CliResult(const CliResult& cr);
	CliResult(CliResult&& cr);
	CliResult(const std::string& resultStr);

	std::string toString();
	CliResult& operator=(const CliResult& cr);
	CliResult& operator=(const std::string& resultStr);

	uint32_t getCode() const;
	const std::string& getData() const;

private:
	uint32_t m_code;
	std::string m_data;
};

class CliPacket {
public:
	CliPacket();
	CliPacket(const std::string& action, 
		const std::string& obj, const CliMembers& members);
	CliPacket(const CliPacket& pkg);
	CliPacket(CliPacket&& pkg);
	CliPacket(const std::string& cmdLine);
	virtual ~CliPacket();

	bool parse(const std::string &cmdLine);
	void reset();
	std::string toString() const;

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
