/*
 * Settings.h
 *
 *  Created on: 2018年3月7日
 *      Author: liyawu
 */

#ifndef FF_SETTINGS_H_
#define FF_SETTINGS_H_

#include <ff/Exception.h>
#include <ff/Object.h>
#include <ff/Singleton.h>
#include <ff/Variant.h>

#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>

NS_FF_BEG

EXCEPTION_DEF(SettingsException);

enum class SettingChangeEvent : uint8_t { Add, Del, Changed };
using SettingChangedFunc =
    std::function<void(SettingChangeEvent event, const std::string& key,
                       const Variant& newValue, const Variant& oldValue)>;

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
  void onChanged(SettingChangedFunc func);

  std::set<std::string> getKeys() const;

  virtual bool saveToFile(const std::string& file);
  virtual bool loadFromFile(const std::string& file);

 private:
  typedef std::map<std::string, Variant> ValueMap;
  ValueMap m_values;
  mutable std::recursive_mutex m_mutex;
  std::list<SettingChangedFunc> m_changedFuncs;
};

using GSettings = Singleton<Settings>;

NS_FF_END

#define G_VAL(name, defVal) GSettings::getInstance()->getValue(name, defVal)
#define S_VAL(name, val) GSettings::getInstance()->setValue(name, val)
#define RM_VAL(name) GSettings::getInstance()->removeValue(name)
#define VAL_CHD(func) GSettings::getInstance()->onChanged(func)
#define VAL_CLR GSettings::getInstance()->clear

#endif /* FF_SETTINGS_H_ */
