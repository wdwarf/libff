/*
 * String_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/String.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <vector>

#include "TestDef.h"

using namespace std;
USE_NS_FF

TEST(StringTest, StringTest) {
  String s = "   test string...   ";
  cout << "s: " << s << endl;
  EXPECT_TRUE(s.equals("   test string...   "));
  EXPECT_TRUE(s.equals("   Test String...   ", true));

  String tsl = s.trimLeft();
  EXPECT_TRUE(tsl.equals("test string...   "));
  EXPECT_TRUE(tsl.equals("Test String...   ", true));

  String tsr = s.trimRight();
  EXPECT_TRUE(tsr.equals("   test string..."));
  EXPECT_TRUE(tsr.equals("   Test String...", true));

  String ts = s.trim();
  EXPECT_TRUE(ts.equals("test string..."));
  EXPECT_TRUE(ts.equals("Test String...", true));

  String upper = ts.toUpper();
  EXPECT_TRUE(upper.equals("TEST STRING..."));
  String lower = upper.toLower();
  EXPECT_TRUE(lower.equals("test string..."));

  String replacedStr = ts.replace("test", "New");
  EXPECT_TRUE(replacedStr.equals("New string..."));
  String replacedStr2 = replacedStr.replace("new", "test", true);
  EXPECT_TRUE(replacedStr2.equals("test string..."));

  String replacedStr3 = replacedStr2.replaceAll(".", "~~~");
  EXPECT_TRUE(replacedStr3.equals("test string~~~~~~~~~"));

  EXPECT_EQ(4, replacedStr3.indexOf(" "));

  auto v = replacedStr3.split(IsAnyOf(" "));
  vector<string> v2{"test", "string~~~~~~~~~"};
  EXPECT_TRUE(equal(v.begin(), v.end(), v2.begin()));

  String s1 = "123456";
  EXPECT_EQ("654321", s1.reverse());

  string str = "中文转换";
  cout << "src: " << str << endl;
  cout << "Utf8ToGbk: " << (str = Utf8ToGbk(str)) << endl;
  cout << "GbkToUtf8: " << (str = GbkToUtf8(str)) << endl;
  wstring ws = L"abcdef aaa中文 转转换！@#￥！转转转12142留个脚印工.";

  wcout << L"ws: " << ws << endl;
  str = ToMbs(ws);
  cout << "ToMbs: " << str << endl;
  cout << "Utf8ToGbk: " << Utf8ToGbk(str) << endl;

  string mbs = "abcdef aaa中文 转转换！@#￥！转转转12142留个脚印工.";
  cout << "mbs: " << mbs << endl;
  ws = ToWs(mbs);
  wcout << L"ToWs: " << ws << endl;

  String ss = ws;
  cout << ss << endl;

  string chs = "abcdef aaa中文 转转换！@#￥！转转转12142留个脚印工.";
  cout << "EncodeOf: " << (uint32_t)EncodeOf(chs) << endl;

  // fstream utf8_file("doc/utf8.txt", ios::binary | ios::in);
  // stringstream utf8Str;
  // while(!utf8_file.eof()){
  // 	char buf[1024] = {0};
  // 	utf8_file.read(buf, 1024);
  // 	auto len = utf8_file.gcount();
  // 	utf8Str.write(buf, len);
  // }

  // cout << "Utf8ToGbk: " << Utf8ToGbk(utf8Str.str()) << endl;

  // fstream gbk_file("doc/gbk.txt", ios::binary | ios::in);
  // stringstream gbkStr;
  // while(!gbk_file.eof()){
  // 	char buf[1024] = {0};
  // 	gbk_file.read(buf, 1024);
  // 	auto len = gbk_file.gcount();
  // 	gbkStr.write(buf, len);
  // }

  // cout << "GbkToUtf8: " << GbkToUtf8(gbkStr.str()) << endl;

  // char aa[] = {0X6d, 0X4b, 0X8b, 0Xd5, 0X77, 0Xed, 0X4f, 0Xe1, 0X4e, 0X00};
  uint8_t aa[] = {0X4b, 0X6d, 0Xd5, 0X8b, 0Xed, 0X77,
                  0Xe1, 0X4f, 0X00, 0X4e, 0x00, 0x00};
  cout << ToMbs((const wchar_t*)aa) << endl;

  s = "测试短信一";
  const uint8_t* bb = (const uint8_t*)s.c_str();
  for (int i = 0; i < s.length(); ++i) {
    cout << hex << (uint16_t)bb[i] << ", ";
  }
  cout << endl;

  s = Utf8ToGbk(s);
  cout << s << endl;

  // wcout << ub << endl;
  auto w = ToWs(s);
  cout << w.length() << endl;
  // wcout << w << endl;
  const uint16_t* b = (const uint16_t*)w.c_str();
  for (int i = 0; i < w.length(); ++i) {
    cout << hex << b[i] << ", ";
  }
  cout << endl;

  cout << ToMbs(w) << endl;
}

TEST(StringTest, JoinTest) {
  const char* ss[] = {"1", "2", "3"};
  auto s = Join(ss, ",");
  LOGD << s;

  s = Join(vector<string>{"1", "2", "3"}, ",");
  LOGD << s;

  s = Join(vector<const char*>{"1", "2", "3"}, ",");
  LOGD << s;

  s = Join(list<string>{"1", "2", "3"}, ",");
  LOGD << s;

  s = Join(list<const char*>{"1", "2", "3"}, ",");
  LOGD << s;

  s = Join(set<string>{"1", "2", "3"}, "-");
  LOGD << s;

  s = Join(set<const char*>{"1", "2", "3"}, "--");
  LOGD << s;
}

TEST(StringTest, NumericTest) {
  String s = "100";
  bool ok = false;

  LOGD << s << " toInt=======";
  LOGD << s.toInt(8, &ok);
  LOGD << "ok: " << ok;
  LOGD << s.toInt(10, &ok);
  LOGD << "ok: " << ok;
  LOGD << s.toInt(16, &ok);
  LOGD << "ok: " << ok;

  LOGD << s << " toLongLong=======";
  LOGD << s.toLongLong(8, &ok);
  LOGD << "ok: " << ok;
  LOGD << s.toLongLong(10, &ok);
  LOGD << "ok: " << ok;
  LOGD << s.toLongLong(16, &ok);
  LOGD << "ok: " << ok;

  LOGD << s << " toUInt=======";
  LOGD << s.toUInt(8, &ok);
  LOGD << "ok: " << ok;
  LOGD << s.toUInt(10, &ok);
  LOGD << "ok: " << ok;
  LOGD << s.toUInt(16, &ok);
  LOGD << "ok: " << ok;

  LOGD << s << " toULongLong=======";
  LOGD << s.toULongLong(8, &ok);
  LOGD << "ok: " << ok;
  LOGD << s.toULongLong(10, &ok);
  LOGD << "ok: " << ok;
  LOGD << s.toULongLong(16, &ok);
  LOGD << "ok: " << ok;

  s = "3.141592653";
  LOGD << s << " toFloat=======";
  LOGD << s.toFloat(&ok);
  LOGD << s << " toDouble=======";
  LOGD << "ok: " << ok;
  LOGD << s.toDouble(&ok);
  LOGD << s << " toLongDouble=======";
  LOGD << "ok: " << ok;
  LOGD << s.toLongDouble(&ok);
  LOGD << "ok: " << ok;

  s = "xyz";
  LOGD << s << " toInt=======";
  LOGD << s.toInt(10, &ok);
  LOGD << "ok: " << ok;
  LOGD << s << " toDouble=======";
  LOGD << s.toDouble(&ok);
  LOGD << "ok: " << ok;

  LOGD << "number: " << String::number(1024, 8, 8);
  LOGD << "number: " << String::number(1024, 8, 10);
  LOGD << "number: " << String::number(1024, 8, 16, '*');
}

TEST(StringTest, StringListTest) {
  StringList sl;
  sl << "str1"
     << "str2"
     << "str3";
  sl.push_back("str4");
  ASSERT_EQ(4, sl.size());
  ASSERT_EQ("str1", sl[0]);
  ASSERT_EQ("str2", sl[1]);
  ASSERT_EQ("str3", sl[2]);
  ASSERT_EQ("str1.str2.str3.str4", sl.join("."));
  ASSERT_EQ(-1, sl.indexOf("str0"));
  ASSERT_EQ(1, sl.indexOf("str2"));
  ASSERT_EQ(1, sl.indexOf("sTr2", true));
  ASSERT_FALSE(sl.contains("str0"));
  ASSERT_TRUE(sl.contains("str1"));
  ASSERT_TRUE(sl.contains("sTr1", true));
  ASSERT_TRUE(sl.contains("str2"));
  ASSERT_TRUE(sl.contains("sTr2", true));
  ASSERT_TRUE(sl.contains("str3"));
  ASSERT_TRUE(sl.contains("sTr3", true));
}