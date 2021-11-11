/*
 * Settings.cpp
 *
 *  Created on: 2018年3月7日
 *      Author: liyawu
 */

#include <errno.h>
#include <ff/Base64.h>
#include <ff/Settings.h>
#include <ff/String.h>
#include <json/json.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

NS_FF_BEG

Settings::Settings() {
  //
}

Settings::~Settings() {
  //
}

const Variant &Settings::getValue(const std::string &key) const {
  std::lock_guard<recursive_mutex> lk(this->m_mutex);
  ValueMap::const_iterator it = this->m_values.find(key);
  if (it == this->m_values.end()) {
    THROW_EXCEPTION(SettingsException, "[" + key + "] not found.", -1);
  }
  return it->second;
}

Variant Settings::getValue(const std::string &key,
                           const Variant &defaultValue) const {
  std::lock_guard<recursive_mutex> lk(this->m_mutex);
  ValueMap::const_iterator it = this->m_values.find(key);
  if (it == this->m_values.end()) {
    return defaultValue;
  }
  return it->second;
}

void Settings::setValue(const std::string &key, const Variant &value) {
  if (this->m_changedFuncs.empty()) {
    std::lock_guard<recursive_mutex> lk(this->m_mutex);
    this->m_values[key] = value;
    return;
  }

  Variant oldValue;
  SettingChangeEvent event = SettingChangeEvent::Changed;

  {
    std::lock_guard<recursive_mutex> lk(this->m_mutex);
    auto it = this->m_values.find(key);
    if (it == this->m_values.end()) {
      this->m_values[key] = value;
      event = SettingChangeEvent::Add;
    }

    if (it != this->m_values.end()) {
      oldValue = it->second;
      it->second = value;
    }
  }

  for (auto &func : this->m_changedFuncs) func(event, key, value, oldValue);
}

bool Settings::hasValue(const std::string &key) const {
  std::lock_guard<recursive_mutex> lk(this->m_mutex);
  return (this->m_values.find(key) != this->m_values.end());
}

void Settings::removeValue(const std::string &key) {
  if (this->m_changedFuncs.empty()) {
    std::lock_guard<recursive_mutex> lk(this->m_mutex);
    this->m_values.erase(key);
    return;
  }

  Variant oldValue;
  {
    std::lock_guard<recursive_mutex> lk(this->m_mutex);
    auto it = this->m_values.find(key);
    if (it == this->m_values.end()) return;
    oldValue = it->second;
    this->m_values.erase(it);
  }

  for (auto &func : this->m_changedFuncs)
    func(SettingChangeEvent::Del, key, Variant(), oldValue);
}

const Variant &Settings::operator[](const std::string &key) const {
  return this->getValue(key);
}

void Settings::clear() {
  if (this->m_changedFuncs.empty()) {
    std::lock_guard<recursive_mutex> lk(this->m_mutex);
    this->m_values.clear();
    return;
  }

  ff::Settings::ValueMap values;

  {
    std::lock_guard<recursive_mutex> lk(this->m_mutex);
    values = move(this->m_values);
  }

  for (auto &p : values) {
    for (auto &func : this->m_changedFuncs)
      func(SettingChangeEvent::Del, p.first, Variant(), p.second);
  }
}

void Settings::onChanged(SettingChangedFunc func) {
  std::lock_guard<recursive_mutex> lk(this->m_mutex);
  this->m_changedFuncs.push_back(func);
}

std::set<std::string> Settings::getKeys() const {
  std::set<std::string> keys;

  std::lock_guard<recursive_mutex> lk(this->m_mutex);
  for (ValueMap::const_iterator it = this->m_values.begin();
       it != this->m_values.end(); ++it) {
    keys.insert(it->first);
  }

  return keys;
}

bool Settings::saveToFile(const std::string &file) {
  std::lock_guard<recursive_mutex> lk(this->m_mutex);
  Json::Value root;
  for (auto p : this->m_values) {
    auto names =
        Split(p.first, IsAnyOf("."), StringCompressType::RemoveEmptyString);

    Json::Value *pRoot = &root;
    for (auto name : names) {
      pRoot = &(*pRoot)[name];
    }

    switch (p.second.getVt()) {
      case VariantType::BOOLEAN:
        *pRoot = (bool)p.second;
        break;
      case VariantType::UCHAR:
      case VariantType::USHORT:
      case VariantType::UINT:
      case VariantType::ULONG:
      case VariantType::ULONGLONG:
        *pRoot = (Json::UInt64)p.second;
        break;
      case VariantType::CHAR:
      case VariantType::SHORT:
      case VariantType::INT:
      case VariantType::LONG:
      case VariantType::LONGLONG:
        *pRoot = (Json::Int64)p.second;
        break;
      case VariantType::FLOAT:
      case VariantType::DOUBLE:
        *pRoot = (double)p.second;
        break;
      default:
        *pRoot = p.second.toString();
        break;
    }
  }

  Json::StreamWriterBuilder builder;
  std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
  if (!writer) return false;

  fstream f(file, ios::out | ios::trunc);
  return (0 == writer->write(root, &f));
}

static void setVal_(Settings &settings, const std::string &name,
                    Json::Value &node) {
  if (node.isString()) {
    settings.setValue(name, node.asString());
    return;
  }
  if (node.isBool()) {
    settings.setValue(name, node.asBool());
    return;
  }
  if (node.isInt() || node.isInt64()) {
    settings.setValue(name, node.asInt64());
    return;
  }
  if (node.isUInt() || node.isUInt64()) {
    settings.setValue(name, node.asUInt64());
    return;
  }
  if (node.isDouble()) {
    settings.setValue(name, node.asDouble());
    return;
  }

  settings.setValue(name, node.asString());
}

static void setVal(Settings &settings, const std::string &baseName,
                   Json::Value &node) {
  for (auto name : node.getMemberNames()) {
    string newName = name;
    if (!baseName.empty()) newName = baseName + "." + newName;

    auto n = node[name];
    if (n.isObject()) {
      setVal(settings, newName, n);
      continue;
    }

    if (n.isArray()) {
      for (int i = 0; i < n.size(); ++i) {
        setVal(settings, newName + "." + std::to_string(i), n[i]);
      }
      continue;
    }

    setVal_(settings, newName, n);
  }
}

bool Settings::loadFromFile(const std::string &file) {
  Json::CharReaderBuilder builder;

  ifstream in(file);
  Json::Value root;
  std::string errInfo;
  if (!Json::parseFromStream(builder, in, &root, &errInfo)) return false;

  setVal(*this, "", root);
  return true;
}

NS_FF_END
