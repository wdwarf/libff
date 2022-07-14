/**
 * @file CmdLineParser_test.cpp
 * @auth DuckyLi
 * @date 2022-07-04 11:34:29
 * @description
 */

#include <ff/DataService.h>
#include <ff/Settings.h>
#include <ff/thirdparty/json/json.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>

#include "TestDef.h"

using namespace std;
USE_NS_FF

STRUCT_DEF_BEGIN(MyTest)
MEMBER_DEF_FLOAT(f);
MEMBER_DEF_DOUBLE(d);
STRUCT_DEF_END

struct C {
  C(int i) : c(i) {}
  int c;
};
const char* str =
    "[\n  {\n    \"name\": \"DeviceInfo\",\n    \"isSingle\": true,\n    "
    "\"isTemp\": true,\n    \"members\": [\n      {\n        \"name\": "
    "\"serialNumber\",\n        \"type\": \"string[32]\",\n        \"isPK\": "
    "false,\n        \"defaultValue\": \"\",\n        \"validationRules\": "
    "\"R:^[0-9A-Z]{16}$\"\n      },\n      {\n        \"name\": "
    "\"bootCount\",\n        \"type\": \"uint32\",\n        \"isPK\": false,\n "
    "       \"defaultValue\": \"0\",\n        \"validationRules\": "
    "\"N:0..0xffffff\"\n      },\n      {\n        \"name\": \"available\",\n  "
    "      \"type\": \"bool\",\n        \"isPK\": false,\n        "
    "\"defaultValue\": \"false\",\n        \"validationRules\": \"\"\n      "
    "}\n    ]\n  },\n  {\n    \"name\": \"SysConfig\",\n    \"isSingle\": "
    "false,\n    \"isTemp\": false,\n    \"members\": [\n\n      {\n        "
    "\"name\": \"variable\",\n        \"type\": \"string[64]\",\n        "
    "\"isPK\": true,\n        \"defaultValue\": \"\",\n        "
    "\"validationRules\": \"\"\n      },\n      {\n        \"name\": "
    "\"value\",\n        \"type\": \"string[128]\",\n        \"isPK\": "
    "false,\n        \"defaultValue\": \"\",\n        \"validationRules\": "
    "\"\"\n      },\n      {\n        \"name\": \"set_time\",\n        "
    "\"type\": \"string[32]\",\n        \"isPK\": false,\n        "
    "\"defaultValue\": \"\",\n        \"validationRules\": \"\"\n      }\n    "
    "],\n\n    \"defaultRows\": [\n      {\n        \"variable\": "
    "\"last_boot_time\",\n        \"value\": \"2022-07-06 00:00:00\"\n      "
    "},\n      {\n        \"variable\": \"log_level\",\n        \"value\": "
    "\"info\"\n      }\n    ]\n  }\n]";
TEST(DataServiceTest, DataServiceTest) {
  cout << str << endl;
  JsonStrDataEntityLoader loader(str);
  std::vector<DataEntityInfo> entities;
  loader.load(entities);
  for (auto& entity : entities) {
    cout << "name: " << entity.name << endl;
    cout << "isSingle: " << entity.isSingle << endl;
    cout << "isTemp: " << entity.isTemp << endl;
  }

  MyTest mt;
  mt.f(3.14159f);
  mt.d(3.14159);
  cout << "sizeof(MyTest): " << sizeof(MyTest) << endl;
  cout << mt.f() << ", " << mt.d() << endl;

  MyTest mt2;
  bit_copy(mt2, mt);
  cout << mt2.f() << ", " << mt2.d() << endl;

  DataServicePacket pkg;
  cout << pkg.generate(1, DsFrameType::Rsp, 0x01, 0, &mt2, MyTest::Size())
       << endl;
  auto hdr = pkg.header();
  cout << "id: " << hdr->id() << endl
       << "length: " << hdr->length() << endl
       << "frameType: " << hdr->frameType() << endl
       << hex << "opCode: 0x" << hdr->opCode() << endl
       << "option: 0x" << hdr->option() << endl
       << "dataHash: 0x" << hdr->dataHash() << endl
       << dec;
  DataServicePacket pkg2;
  cout << pkg2.parse(hdr, hdr->length()) << endl;

  hdr = pkg2.header();
  cout << "id: " << hdr->id() << endl
       << "length: " << hdr->length() << endl
       << "frameType: " << hdr->frameType() << endl
       << hex << "opCode: 0x" << hdr->opCode() << endl
       << "option: 0x" << hdr->option() << endl
       << "dataHash: 0x" << hdr->dataHash() << endl
       << dec;

cout << "=============================" << endl;
  DsPacketHelper helper;
  helper.append("1234567890", 10);
  helper.append(hdr, hdr->length());
  helper.append(hdr, hdr->length());
  helper.append("123456780", 9);
  helper.append(hdr, hdr->length());
  helper.append("1234567890", 10);
  DsPacketPtr p;
  while (p = helper.getPackage()) {
    hdr = p->header();
    cout << "id: " << hdr->id() << endl
         << "length: " << hdr->length() << endl
         << "frameType: " << hdr->frameType() << endl
         << hex << "opCode: 0x" << hdr->opCode() << endl
         << "option: 0x" << hdr->option() << endl
         << "dataHash: 0x" << hdr->dataHash() << endl
         << dec;
  }
}
