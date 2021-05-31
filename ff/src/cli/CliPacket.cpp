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

NS_FF_BEG

CliResult::CliResult() : m_code(0) {

}

CliResult::CliResult(const std::string& resultStr){
	stringstream str;
	str << resultStr;
	str >> this->m_code;
	str >> this->m_data;
	ReplaceAll(this->m_data, "\\n", "\n");
}

CliResult::CliResult(uint32_t code, const std::string& data) 
	: m_code(code), m_data(data) {
}

CliResult::CliResult(const CliResult& cr){
	this->m_code = cr.m_code;
	this->m_data = cr.m_data;
}

CliResult::CliResult(CliResult&& cr){
	this->m_code = cr.m_code;
	this->m_data = std::move(cr.m_data);
	cr.m_code = 0;
}

CliResult& CliResult::operator=(const CliResult& cr){
	this->m_code = cr.m_code;
	this->m_data = cr.m_data;
	return *this;
}

CliResult& CliResult::operator=(const std::string& resultStr){
	stringstream str;
	str << resultStr;
	str >> this->m_code;
	str >> this->m_data;
	ReplaceAll(this->m_data, "\\n", "\n");
	return *this;
}

std::string CliResult::toString(){
	stringstream str; 
	str << this->m_code << " " << ReplaceAllCopy(this->m_data, "\n", "\\n");
	return str.str();
}

uint32_t CliResult::getCode() const{
	return this->m_code;
}

const std::string& CliResult::getData() const{
	return this->m_data;
}


//===============================================


CliPacket::CliPacket() {
}

CliPacket::CliPacket(const std::string& action, 
	const std::string& obj, const CliMembers& members) 
	: m_action(action), m_obj(obj), m_members(members) {

}

CliPacket::CliPacket(const CliPacket& pkg){
	this->m_action = pkg.m_action;
	this->m_obj = pkg.m_obj;
	this->m_members = pkg.m_members;
}

CliPacket::CliPacket(CliPacket&& pkg){
	this->m_action = std::move(pkg.m_action);
	this->m_obj = std::move(pkg.m_obj);
	this->m_members = std::move(pkg.m_members);
}

CliPacket::CliPacket(const std::string& cmdLine){
	this->parse(cmdLine);
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

bool CliPacket::parse(const std::string &cmdLine) {
	this->reset();
	string cmdTmp = TrimCopy(cmdLine);
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

std::string CliPacket::toString() const{
	if(this->m_action.empty() || this->m_obj.empty()) return "";

	stringstream cmdLine;

	cmdLine << this->m_action;
	if(!this->m_obj.empty()){
		cmdLine << " " << this->m_obj;
	}

	if(!this->m_members.empty())
		cmdLine << " ";

	auto it = this->m_members.begin();
	for(size_t i = 0; i < this->m_members.size(); ++i, ++it){
		cmdLine << it->first << "=";
		auto& value = it->second;
		auto vt = value.getVt();

		bool strType = false;
		if(VariantType::STRING == vt || VariantType::CARRAY == vt){
			strType = true;
		}

		if(strType)
			cmdLine << "\"";
		cmdLine << value.toString();
		if(strType)
			cmdLine << "\"";

		if(i + 1 < this->m_members.size()){
			cmdLine << ",";
		}
	}

	cmdLine << ";";
	return cmdLine.str();
}

NS_FF_END
