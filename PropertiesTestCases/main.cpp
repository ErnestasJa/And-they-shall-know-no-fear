#include "precomp.h"
#include "properties\property.h"
#include "properties\property_container.h"

#include "gtest/gtest.h"

class PropertyTests : public ::testing::Test
{
protected:

protected:
	PropertyTests()
	{
		PropertyContainer::register_properties();
	}

	virtual ~PropertyTests()
	{
    
	}

	virtual void SetUp()
	{

	}

	virtual void TearDown()
	{
   
	}
};

TEST_F(PropertyTests, TestExistance)
{
	PropertyContainer p;
	p.add_property<uint32_t>("x");

	ASSERT_TRUE(p.has_property("x"));
	ASSERT_TRUE(p.has_property("x",EPT_UINT32));
	ASSERT_FALSE(p.has_property("y"));
}

TEST_F(PropertyTests, TestSerialization) 
{
	
	PropertyContainer p,c;

	p.add_property<uint32_t>("x", 15);
	p.add_property<std::string>("y", "hello world!");

	clan::File f("test.dat",clan::File::create_always,clan::File::access_write);
	p.serialize(f);
	f.close();
  
	clan::File f2("test.dat",clan::File::open_existing,clan::File::access_read);
	c.deserialize(f2);
	f2.close();

	c.get_property<uint32_t>("x");
	c.get_property<std::string>("y");

	ASSERT_EQ(p.get_property<uint32_t>("x"),c.get_property<uint32_t>("x"));
	ASSERT_EQ(p.get_property<std::string>("y"),c.get_property<std::string>("y"));
}

TEST_F(PropertyTests, NumericValueSetGetTest)
{
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

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();

	std::getwchar();
	return ret;
}