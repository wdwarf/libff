/*
 * Cli_test.cpp
 *
 *  Created on: 2021-05-31
 *      Author: ducky
 */

#include <ff/CliPacket.h>
#include <gtest/gtest.h>
#include "TestDef.h"
#include <iostream>
#include <vector>

using namespace std;
USE_NS_FF

TEST(CliTest, CliPacket){
  cout << CliPacket("list config;").toString() << endl;
  cout << CliPacket("take pictures saveDir=\"d:/testdir\",oneDirPerCamera=false;").toString() << endl;
  
  CliPacket pkg("get", "id", { {"name", "liyawu"}, { "age", "30" } });
  cout << pkg.toString() << endl;

  CliPacket pkg2(pkg.toString());
  cout << pkg2.toString() << endl;

  CliPacket pkgTmp("reboot reason=\"test\";");
  cout << pkgTmp.toString() << endl;

  CliPacket pkg3(move(pkg));
  cout << pkg3.toString() << endl;
  cout << pkg.toString() << endl;

  CliPacket pkg4("take", "pic", {});
  cout << "pkg4: " << pkg4.toString() << endl;
  
  CliPacket pkg5("reboot", "", { { "reason", "menual \"reset\"." } });
  cout << "pkg5: " << pkg5.toString() << endl;
  for(auto& p : pkg5.getMembers()){
    cout << p.first << " = " << p.second << endl;
  }

  CliPacket pkg6(pkg5.toString());
  cout << "pkg6: " << pkg6.toString() << endl;

  CliPacket pkg7 = pkg6;
  cout << "pkg7: " << pkg6.toString() << endl;

  CliResult result(200, "success\n\"123\"");
  cout << result.toString() << endl;
  cout << result.getCode() << endl;
  cout << result.getData() << endl;

  result = CliResult(result.toString());
  cout << result.toString() << endl;
  cout << result.getCode() << endl;
  cout << result.getData() << endl;

  result = result.toString();
  cout << result.toString() << endl;
  cout << result.getCode() << endl;
  cout << result.getData() << endl;

  CliPacket pkg8;
  cout << pkg8.parse("reboot") << endl;
  cout << pkg8.toString() << endl;
  cout << pkg8.parse("reboot reason=\"undefined error.\"") << endl;
  cout << pkg8.toString() << endl;
  cout << pkg8.parse("reboot device1 reason=\"device1 failed.\",tick=1234567890;") << endl;
  cout << pkg8.toString() << endl;
  for(auto& m : pkg8.getMembers()){
    cout << m.first << ": " << m.second << endl;
  }

}
