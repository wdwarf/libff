/*
 * CliService.cpp
 *
 *  Created on: Mar 25, 2020
 *      Author: liyawu
 */

#include <ff/CliService.h>
#include <ff/String.h>

using namespace std;

namespace NS_FF {

CliService::CliService() {
}

CliService::~CliService() {
}

CliService* CliService::registerCliHandler(const std::string &action,
		const std::string &obj, CliHandlerFunc handler) {
	lock_guard<mutex> lk(this->m_handlerMutex);
	this->m_cliHandlers[make_pair(ToLowerCopy(action), ToLowerCopy(obj))] =
			handler;
	return this;
}

string CliService::handleCmd(const CliPacket &pkg) {
	auto action = ToLowerCopy(pkg.getAction());
	auto obj = ToLowerCopy(pkg.getObj());

	CliHandlerFunc func;

	{
		lock_guard<mutex> lk(this->m_handlerMutex);
		auto it = this->m_cliHandlers.find(make_pair(action, obj));
		if (it == m_cliHandlers.end() && !obj.empty()) {
			it = this->m_cliHandlers.find(make_pair(action, ""));
		}

		if (it != m_cliHandlers.end()) {
			func = it->second;
		}
	}

	return func ? func(pkg) : "";
}

} /* namespace NS_FF */
