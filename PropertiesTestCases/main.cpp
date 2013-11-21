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

	ASSERT_EQ(p.get_property<uint32_t>("x"),c.get_property<uint32_t>("x"));
	ASSERT_EQ(p.get_property<std::string>("y"),c.get_property<std::string>("y"));
}

TEST_F(PropertyTests, TestXmlSerialization) 
{
	PropertyContainer p,c,p3;
	p3.add_property<uint32_t>("next_level_property", 12345);
	p3.add_property<std::string>("right", "here");

	p.add_property<uint32_t>("x", 15);
	p.add_property<std::string>("y", "hello world!");
	p.add_property<PropertyContainer>("other props")=p3;

	clan::File f("test_xml.dat",clan::File::create_always,clan::File::access_write);
	clan::DomDocument d;
	p.xml_serialize(d,d.get_first_child().to_element());
	d.save(f);
	f.close();
  
	clan::File f2("test_xml.dat",clan::File::open_existing,clan::File::access_read);
	clan::DomDocument d2;
	d2.load(f2);
	c.xml_deserialize(d2.get_first_child().to_element());
	f2.close();


	PropertyContainer pc = c.get_property<PropertyContainer>("other props");
	
	ASSERT_TRUE(pc.has_property("next_level_property"));
	ASSERT_TRUE(pc.has_property("right"));

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

TEST_F(PropertyTests, SharedPropertyTest)
{
	PropertyContainer p,p2;

	p.add_property<uint32_t>("x",15);
	p.add_property<uint32_t>("y")=20;

	p2 = p;

	auto z = p.add_property<uint32_t>("z");
	z=25;

	ASSERT_TRUE(p.has_property("x"));
	ASSERT_TRUE(p.has_property("y"));
	ASSERT_TRUE(p.has_property("z"));

	ASSERT_TRUE(p2.has_property("x"));
	ASSERT_TRUE(p2.has_property("y"));
	ASSERT_FALSE(p2.has_property("z"));

	ASSERT_EQ(15,p2.get_property<uint32_t>("x"));
	ASSERT_EQ(20,p2.get_property<uint32_t>("y"));
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();

	std::getwchar();
	return ret;
}