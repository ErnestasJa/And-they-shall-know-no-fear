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
	PropertyContainer p,c,p3;
	p3.add_property<uint32_t>("next_level_property", 12345);
	p3.add_property<std::string>("right", "here");

	p.add_property<uint32_t>("x", 15);
	p.add_property<std::string>("y", "hello world!");
	p.add_property<PropertyContainer>("other props")=p3;

	clan::File f("test.dat",clan::File::create_always,clan::File::access_write);
	p.serialize(f);
	f.close();
  
	clan::File f2("test.dat",clan::File::open_existing,clan::File::access_read);
	c.deserialize(f2);
	f2.close();

	PropertyContainer pc = c.get_property<PropertyContainer>("other props");
	
	ASSERT_TRUE(pc.has_property("next_level_property"));
	ASSERT_TRUE(pc.has_property("right"));

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

TEST_F(PropertyTests, TestNetSerialization) 
{
	PropertyContainer pc_send, p3, pc_receive;

	clan::NetGameEvent msg("msg");
	clan::NetGameEventValue val(clan::NetGameEventValue::complex);
	

	p3.add_property<uint32_t>("next_level_property", 12345);
	p3.add_property<std::string>("right", "here");

	pc_send.add_property<uint32_t>("x", 15);
	pc_send.add_property<std::string>("y", "hello world!");
	pc_send.add_property<PropertyContainer>("other props")=p3;

	/// send, receive
	pc_send.net_serialize(val,false);
	msg.add_argument(val);

	clan::NetGameEventValue v = msg.get_argument(0);
	pc_receive.net_deserialize(v);

	///test
	ASSERT_TRUE(pc_receive.has_property("x"));
	ASSERT_TRUE(pc_receive.has_property("y"));
	ASSERT_TRUE(pc_receive.has_property("other props"));

	PropertyContainer pc = pc_receive.get_property<PropertyContainer>("other props");
	ASSERT_TRUE(pc.has_property("next_level_property"));
	ASSERT_TRUE(pc.has_property("right"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive.get_property<std::string>("y"));
}

TEST_F(PropertyTests, TestNetMultiEventSerialization) 
{
	PropertyContainer pc_send, p3, pc_receive, pc_receive2;

	clan::NetGameEvent msg("msg"), msg2("msg");
	clan::NetGameEventValue val(clan::NetGameEventValue::complex), val2(clan::NetGameEventValue::complex);
	

	p3.add_property<uint32_t>("next_level_property", 12345);
	p3.add_property<std::string>("right", "here");

	pc_send.add_property<uint32_t>("x", 15);
	pc_send.add_property<std::string>("y", "hello world!");
	pc_send.add_property<PropertyContainer>("other props")=p3;

	/// send, receive
	pc_send.net_serialize(val,false);
	msg.add_argument(val);

	clan::NetGameEventValue v = msg.get_argument(0);
	pc_receive.net_deserialize(v);

	pc_send.net_serialize(val2,false);
	msg2.add_argument(val2);

	clan::NetGameEventValue v2 = msg2.get_argument(0);
	pc_receive2.net_deserialize(v2);

	///test 1st msg
	ASSERT_TRUE(pc_receive.has_property("x"));
	ASSERT_TRUE(pc_receive.has_property("y"));
	ASSERT_TRUE(pc_receive.has_property("other props"));

	PropertyContainer pc = pc_receive.get_property<PropertyContainer>("other props");
	ASSERT_TRUE(pc.has_property("next_level_property"));
	ASSERT_TRUE(pc.has_property("right"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive.get_property<std::string>("y"));

	///test 2nd msg
	ASSERT_TRUE(pc_receive2.has_property("x"));
	ASSERT_TRUE(pc_receive2.has_property("y"));
	ASSERT_TRUE(pc_receive2.has_property("other props"));

	PropertyContainer pc2 = pc_receive2.get_property<PropertyContainer>("other props");
	ASSERT_TRUE(pc2.has_property("next_level_property"));
	ASSERT_TRUE(pc2.has_property("right"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive2.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive2.get_property<std::string>("y"));
}

TEST_F(PropertyTests, TestNetMultiEventOnlyChandedPropertySerialization) 
{
	PropertyContainer pc_send, p3, pc_receive, pc_receive2;

	clan::NetGameEvent msg("msg"), msg2("msg");
	clan::NetGameEventValue val(clan::NetGameEventValue::complex), val2(clan::NetGameEventValue::complex);
	

	p3.add_property<uint32_t>("next_level_property", 12345);
	p3.add_property<std::string>("right", "here");

	pc_send.add_property<uint32_t>("x", 15);
	pc_send.add_property<std::string>("y", "hello world!");
	pc_send.add_property<PropertyContainer>("other props")=p3;

	/// send, receive
	pc_send.net_serialize(val,true);
	msg.add_argument(val);

	clan::NetGameEventValue v = msg.get_argument(0);
	pc_receive.net_deserialize(v);

	pc_send.net_serialize(val2,true);
	msg2.add_argument(val2);

	clan::NetGameEventValue v2 = msg2.get_argument(0);
	pc_receive2.net_deserialize(v2);

	///test 1st msg
	ASSERT_TRUE(pc_receive.has_property("x"));
	ASSERT_TRUE(pc_receive.has_property("y"));
	ASSERT_TRUE(pc_receive.has_property("other props"));

	PropertyContainer pc = pc_receive.get_property<PropertyContainer>("other props");
	ASSERT_TRUE(pc.has_property("next_level_property"));
	ASSERT_TRUE(pc.has_property("right"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive.get_property<std::string>("y"));

	///test 2nd msg
	ASSERT_TRUE(pc_receive2.has_property("x"));
	ASSERT_TRUE(pc_receive2.has_property("y"));
	ASSERT_TRUE(pc_receive2.has_property("other props"));

	PropertyContainer pc2 = pc_receive2.get_property<PropertyContainer>("other props");
	ASSERT_TRUE(pc2.has_property("next_level_property"));
	ASSERT_TRUE(pc2.has_property("right"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive2.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive2.get_property<std::string>("y"));
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