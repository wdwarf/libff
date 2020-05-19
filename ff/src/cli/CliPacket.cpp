/*
 * CliPacket.cpp
 *
 *  Created on: Mar 25, 2020
 *      Author: liyawu
 */

#include <ff/CliPacket.h>
#include <string>
#include <ff/String.h>

using namespace std;

namespace NS_FF {

CliPacket::CliPacket() {
}

CliPacket::~CliPacket() {
}
const std::string& CliPacket::getAction() const {
	return this->m_action;
}

const std::string& CliPacket::getObj() const {
	return this->m_obj;
}

const CliMembers& CliPacket::getMembers() const {
	return this->m_members;
}

bool CliPacket::parse(const std::string &cmd) {
	this->reset();
	string cmdTmp = TrimCopy(cmd);
	if (cmdTmp.empty())
		return false;
	if (';' == cmdTmp[cmdTmp.length() - 1]) {
		cmdTmp = cmdTmp.substr(0, cmdTmp.length() - 1);

		if (cmdTmp.empty())
			return false;
	}

	string::size_type pos = 0;
	auto getWord = [&](string &str) {
		for (; pos < cmdTmp.length(); ++pos) {
			auto c = cmdTmp[pos];
			if (isspace(c))
				break;
			str += c;
		}
	};
	getWord(this->m_action);

	auto jumpOverSpace = [&] {
		for (; pos < cmdTmp.length(); ++pos)
		{
			auto c = cmdTmp[pos];
			if (isspace(c))
			continue;
			break;
		}
	};
	jumpOverSpace();

	getWord(this->m_obj);

	jumpOverSpace();

	string name;
	string value;
	bool parsingName = true; /** 是否正在解析名称 */
	bool isQuoteMode = false; /** 是否是引号模式（即字符串值） */
	bool isTransMode = false; /** 是否是转义模式(\) */
	for (; pos < cmdTmp.length(); ++pos) {
		auto c = cmdTmp[pos];

		if (parsingName) {
			if (',' == c)
				continue;

			if ('=' == c) {
				parsingName = false;
				continue;
			}

			name += c;
			continue;
		}

		if (parsingName)
			return false;

		if (',' == c) {
			if (!isQuoteMode) {
				this->m_members.insert(
						make_pair(TrimCopy(name), Variant(TrimCopy(value))));
				parsingName = true;
				name = "";
				value = "";
				continue;
			}
		}

		if ('"' == c) {
			if (isTransMode) {
				value += c;
				isTransMode = false;
				continue;
			}
			if (!isQuoteMode) {
				isQuoteMode = true;
				continue;
			}

			this->m_members.insert(make_pair(TrimCopy(name), Variant(value)));
			isQuoteMode = false;
			parsingName = true;
			name = "";
			value = "";
			continue;
		}

		if ('\\' == c) {
			if (!isQuoteMode) {
				return false;
			}

			if (!isTransMode) {
				isTransMode = true;
				continue;
			}

			value += c;
			isTransMode = false;
			continue;
		}

		value += c;
		if (pos + 1 == cmdTmp.length()) {
			this->m_members.insert(
					make_pair(TrimCopy(name), Variant(TrimCopy(value))));
			name = "";
			value = "";
			break;
		}
	}

	return true;
}

void CliPacket::reset() {
	this->m_action = "";
	this->m_obj = "";
	this->m_members.clear();
}

} /* namespace NS_FF */
