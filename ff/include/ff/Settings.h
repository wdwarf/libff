/*
 * Settings.h
 *
 *  Created on: 2018年3月7日
 *      Author: liyawu
 */

#ifndef FF_SETTINGS_H_
#define FF_SETTINGS_H_

#include <ff/Object.h>
#include <ff/Exception.h>
#include <ff/Variant.h>
#include <ff/Singleton.h>
#include <mutex>
#include <string>
#include <map>
#include <set>

NS_FF_BEG

EXCEPTION_DEF(SettingsException);

class LIBFF_API Settings {
public:
	Settings();
	virtual ~Settings();

	const Variant& getValue(const std::string& key) const;
	Variant getValue(const std::string& key, const Variant& defaultValue) const;
	void setValue(const std::string& key, const Variant& value);
	bool hasValue(const std::string& key) const;
	void removeValue(const std::string& key);
	const Variant& operator[](const std::string& key) const;
	void clear();

	std::set<std::string> getKeys() const;

	virtual bool saveToFile(const std::string& file);
	virtual bool loadFromFile(const std::string& file);
private:
	typedef std::map<std::string, Variant> ValueMap;
	ValueMap m_values;
	mutable std::mutex m_mutex;
};

using GSettings = Singleton<Settings>;

NS_FF_END

#define G_VAL(name, defVal) GSettings::getInstance()->getValue(name, defVal)
#define S_VAL(name, val) GSettings::getInstance()->setValue(name, val);


#endif /* FF_SETTINGS_H_ */
