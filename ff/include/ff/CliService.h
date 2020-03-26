/*
 * CliService.h
 *
 *  Created on: Mar 25, 2020
 *      Author: u16
 */

#ifndef CLI_CLISERVICE_H_
#define CLI_CLISERVICE_H_

#include <functional>
#include <string>
#include <mutex>
#include <map>
#include <ff/ff_config.h>
#include <ff/CliPacket.h>

namespace NS_FF {

using CliHandlerFunc = std::function<std::string(const CliPacket&)>;

class CliService {
public:
	CliService();
	virtual ~CliService();

	CliService* registerCliHandler(const std::string &action,
			const std::string &obj, CliHandlerFunc handler);

	std::string handleCmd(const CliPacket &pkg);

private:
	mutable std::mutex m_handlerMutex;
	std::map<std::pair<std::string, std::string>, CliHandlerFunc> m_cliHandlers;
};

} /* namespace NS_FF */

#endif /* CLI_CLISERVICE_H_ */
