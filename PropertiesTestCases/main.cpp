#include "precomp.h"
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


TEST_F(PropertyTests, NumericValueSetGetCheck) {
  PropertyContainer p;

  p.add_property<uint32_t>("x",15);
  p.add_property<uint32_t>("y")=20;
  auto z = p.add_property<uint32_t>("z");
  z=25;

  ASSERT_EQ(15,p.get_property<uint32_t>("x"));
  ASSERT_EQ(20,p.get_property<uint32_t>("y"));
  ASSERT_EQ(25,z);
  ASSERT_EQ(25,p.get_property<uint32_t>("z"));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();

  std::getwchar();
  return ret;
}