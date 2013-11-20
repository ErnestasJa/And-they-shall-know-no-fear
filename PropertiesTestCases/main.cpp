#include "precomp.h"
#include <iostream>
#include "properties\property.h"
#include "properties\property_container.h"

#include "gtest/gtest.h"

class PropertyTests : public ::testing::Test {
protected:

protected:
  PropertyTests() {

  }

  virtual ~PropertyTests() {
    
  }

  virtual void SetUp() {

  }

  virtual void TearDown() {
   
  }
};

TEST_F(PropertyTests, CheckExistance) {
  PropertyContainer p;
  p.add_property<uint32_t>("x");

  ASSERT_TRUE(p.has_property("x"));
  ASSERT_FALSE(p.has_property("y"));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();

  std::getwchar();
  return ret;
}