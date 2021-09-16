/*
 * Curl_test.cpp
 *
 *  Created on: 2021-07-06
 *      Author: root
 */

#include <ff/Settings.h>
#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(SettingsTest, SettingsTest) {
  Settings settings;
  settings.onChanged([&settings](SettingChangeEvent event, const std::string& key,
                        const Variant& newValue,
                        const Variant& oldValue) -> void {
    cout << "evt: " << int(event) << ", key: " << key
         << ", newVal: " << newValue << ", oldVal: " << oldValue << endl;
    if(key == "key3") settings.setValue("aaa", oldValue);
  });

  settings.onChanged([](SettingChangeEvent event, const std::string& key,
                        const Variant& newValue,
                        const Variant& oldValue) -> void {
    cout << "event: " << int(event) << ", key: " << key
         << ", newVal: " << newValue << ", oldVal: " << oldValue << endl;
  });

  settings.setValue("key1", 123);
  cout << endl;
  settings.setValue("key1", 456);
  cout << endl;
  settings.setValue("key2", "key2_value");
  cout << endl;
  settings.setValue("key3", 3.141592653);
  cout << endl;
  settings.removeValue("key2");
  cout << endl;
  settings.setValue("key2", "key2_value2");
  cout << endl;
  settings.clear();
  cout << endl;

  cout << "keys: " << settings.getKeys().size() << endl;
}

TEST(SettingsTest, SettingsTest2) {
  VAL_CHD([](SettingChangeEvent event, const std::string& key,
                        const Variant& newValue,
                        const Variant& oldValue) -> void {
    cout << "evt: " << int(event) << ", key: " << key
         << ", newVal: " << newValue << ", oldVal: " << oldValue << endl;
    if(key == "key3") S_VAL("aaa", oldValue);
  });

  VAL_CHD([](SettingChangeEvent event, const std::string& key,
                        const Variant& newValue,
                        const Variant& oldValue) -> void {
    cout << "event: " << int(event) << ", key: " << key
         << ", newVal: " << newValue << ", oldVal: " << oldValue << endl;
  });

  S_VAL("key1", 123);
  cout << endl;
  S_VAL("key1", 456);
  cout << endl;
  S_VAL("key2", "key2_value");
  cout << endl;
  S_VAL("key3", 3.141592653);
  cout << endl;
  RM_VAL("key2");
  cout << endl;
  S_VAL("key2", "key2_value2");
  cout << endl;
  VAL_CLR();
  cout << endl;

  cout << "keys: " << GSettings::getInstance()->getKeys().size() << endl;
}
