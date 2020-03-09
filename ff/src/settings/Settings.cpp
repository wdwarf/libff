/*
 * Settings.cpp
 *
 *  Created on: 2018年3月7日
 *      Author: liyawu
 */

#include <ff/Settings.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include <errno.h>
#include <cstring>

#include <ff/String.h>
#include <ff/Base64.h>

#include <json/json.h>

using namespace std;

namespace NS_FF {

Settings::Settings() {
	//

}

Settings::~Settings() {
	//
}

const Variant& Settings::getValue(const std::string& key) const {
	std::lock_guard<mutex> lk(this->m_mutex);
	ValueMap::const_iterator it = this->m_values.find(key);
	if (it == this->m_values.end()) {
		THROW_EXCEPTION(SettingsException, "[" + key + "] not found.", -1);
	}
	return it->second;
}

Variant Settings::getValue(const std::string& key,
		const Variant& defaultValue) const {
	std::lock_guard<mutex> lk(this->m_mutex);
	ValueMap::const_iterator it = this->m_values.find(key);
	if (it == this->m_values.end()) {
		return defaultValue;
	}
	return it->second;
}

void Settings::setValue(const std::string& key, const Variant& value) {
	std::lock_guard<mutex> lk(this->m_mutex);
	this->m_values[key] = value;
}

bool Settings::hasValue(const std::string& key) const {
	std::lock_guard<mutex> lk(this->m_mutex);
	return (this->m_values.find(key) != this->m_values.end());
}

void Settings::removeValue(const std::string& key) {
	std::lock_guard<mutex> lk(this->m_mutex);
	this->m_values.erase(key);
}

const Variant& Settings::operator[](const std::string& key) const {
	return this->getValue(key);
}

void Settings::clear() {
	std::lock_guard<mutex> lk(this->m_mutex);
	this->m_values.clear();
}

std::set<std::string> Settings::getKeys() const {
	std::set<std::string> keys;

	std::lock_guard<mutex> lk(this->m_mutex);
	for (ValueMap::const_iterator it = this->m_values.begin();
			it != this->m_values.end(); ++it) {
		keys.insert(it->first);
	}

	return keys;
}

void Settings::saveToFile(const std::string& file) {
	std::lock_guard<mutex> lk(this->m_mutex);
	fstream f;
	f.open(file.c_str(), ios::out | ios::trunc);
	if (!f.is_open()) {
		THROW_EXCEPTION(SettingsException,
				"file[" + file + "] open failed. " + strerror(errno), errno);
	}

	for (ValueMap::iterator it = this->m_values.begin();
			it != this->m_values.end(); ++it) {
		f << Base64::Encrypt(it->first) << ":"
				<< Base64::Encrypt(it->second.toString()) << endl;
	}

	f.close();
}

void Settings::loadFromFile(const std::string& file) {
	std::lock_guard<mutex> lk(this->m_mutex);
	fstream f;
	f.open(file.c_str(), ios::in);
	if (!f.is_open()) {
		THROW_EXCEPTION(SettingsException,
				"file[" + file + "] open failed. " + strerror(errno), errno);
	}

	while (!f.eof()) {
		string line;
		std::getline(f, line);
		Trim(line);
		if (line.empty())
			continue;

		string::size_type pos = line.find(":");
		if (pos == string::npos)
			continue;

		string key = line.substr(0, pos);
		string value = line.substr(pos + 1);
		stringstream strKey;
		stringstream strValue;
		Base64::Decrypt(strKey, key);
		Base64::Decrypt(strValue, value);

		key = strKey.str();
		value = strValue.str();

		this->m_values.insert(make_pair(key, value));
	}

	f.close();
}

} /* namespace NS_FF */

