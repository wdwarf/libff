/*
 * Variant_test.cpp
 *
 *  Created on: Aug 2, 2019
 *      Author: root
 */

#include <ff/Variant.h>
#include <gtest/gtest.h>
#include "TestDef.h"
#include <iostream>

using namespace std;
using namespace NS_FF;

TEST(TestVariant, TestVariant){
	Variant v = 12;
	EXPECT_EQ(VariantType::INT, v.getVt());
	Variant v2 = v;
	LOGD << (v == v2);
	EXPECT_EQ(v, v2);

	LOGD << "buf: " << v.toBuffer().toHexString();
}
