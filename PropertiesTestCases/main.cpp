#include "precomp.h"
#include "properties\property.h"
#include "properties\property_container.h"

#include "net\message.h"

#include "gtest/gtest.h"


class PC_OtherData: public PropertyContainer
{
public:
	Property<uint32_t> w;
	Property<std::string> z;

	PC_OtherData()
	{
		w = add_property<uint32_t>("w");
		z = add_property<std::string>("z", "halo");
	}
protected:

};

class PC_Data: public PropertyContainer
{
public:
	Property<uint32_t> x;
	Property<std::string> y;
	Property<PropertyContainer> other_props;

	PC_Data()
	{
		x = add_property<uint32_t>("x", 15);
		y = add_property<std::string>("y", "hello world!");
		other_props = add_property<PropertyContainer>("other_data",PC_OtherData());
	}
protected:

};


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
	PC_Data p;

	ASSERT_TRUE(p.has_property("x"));
	ASSERT_TRUE(p.has_property("x",EPT_UINT32));
	ASSERT_TRUE(p.has_property("y"));
	ASSERT_TRUE(p.has_property("other_data"));
	ASSERT_FALSE(p.has_property("z"));
}

TEST_F(PropertyTests, TestSerialization) 
{
	PC_Data p, deserialized;

	clan::File f("test.dat",clan::File::create_always,clan::File::access_write);
	p.serialize(f);
	f.close();
  
	clan::File f2("test.dat",clan::File::open_existing,clan::File::access_read);
	deserialized.deserialize(f2);
	f2.close();

	PropertyContainer pc = p.get_property<PropertyContainer>("other_data");
	
	ASSERT_TRUE(pc.has_property("w"));
	ASSERT_TRUE(pc.has_property("z"));

	ASSERT_EQ(p.get_property<uint32_t>("x"),deserialized.get_property<uint32_t>("x"));
	ASSERT_EQ(p.get_property<std::string>("y"),deserialized.get_property<std::string>("y"));
}

TEST_F(PropertyTests, TestXmlSerialization) 
{
	PC_Data p, deserialized;

	clan::File f("test_xml.dat",clan::File::create_always,clan::File::access_write);
	clan::DomDocument d;
	p.xml_serialize(d,d.get_first_child().to_element());
	d.save(f);
	f.close();
  
	clan::File f2("test_xml.dat",clan::File::open_existing,clan::File::access_read);
	clan::DomDocument d2;
	d2.load(f2);
	deserialized.xml_deserialize(d2.get_first_child().to_element());
	f2.close();

	PropertyContainer pc = deserialized.get_property<PropertyContainer>("other_data");
	
	ASSERT_TRUE(pc.has_property("w"));
	ASSERT_TRUE(pc.has_property("z"));

	ASSERT_EQ(p.get_property<uint32_t>("x"),deserialized.get_property<uint32_t>("x"));
	ASSERT_EQ(p.get_property<std::string>("y"),deserialized.get_property<std::string>("y"));
}

TEST_F(PropertyTests, TestNetSerialization) 
{
	PC_Data pc_send, pc_receive;

	clan::NetGameEvent msg("msg");
	clan::NetGameEventValue val(clan::NetGameEventValue::complex);

	/// send, receive
	pc_send.net_serialize(val,false);
	msg.add_argument(val);

	clan::NetGameEventValue v = msg.get_argument(0);
	pc_receive.net_deserialize(v);

	///test
	ASSERT_TRUE(pc_receive.has_property("x"));
	ASSERT_TRUE(pc_receive.has_property("y"));
	ASSERT_TRUE(pc_receive.has_property("other_data"));

	PropertyContainer pc = pc_receive.get_property<PropertyContainer>("other_data");
	ASSERT_TRUE(pc.has_property("w"));
	ASSERT_TRUE(pc.has_property("z"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive.get_property<std::string>("y"));
}

TEST_F(PropertyTests, TestNetMultiEventSerialization) 
{
	PC_Data pc_send, pc_receive, pc_receive2;

	clan::NetGameEvent msg("msg"), msg2("msg");
	clan::NetGameEventValue val(clan::NetGameEventValue::complex), val2(clan::NetGameEventValue::complex);

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
	ASSERT_TRUE(pc_receive.has_property("other_data"));

	PropertyContainer pc = pc_receive.get_property<PropertyContainer>("other_data");
	ASSERT_TRUE(pc.has_property("w"));
	ASSERT_TRUE(pc.has_property("z"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive.get_property<std::string>("y"));

	///test 2nd msg
	ASSERT_TRUE(pc_receive2.has_property("x"));
	ASSERT_TRUE(pc_receive2.has_property("y"));
	ASSERT_TRUE(pc_receive2.has_property("other_data"));

	PropertyContainer pc2 = pc_receive2.get_property<PropertyContainer>("other_data");
	ASSERT_TRUE(pc2.has_property("w"));
	ASSERT_TRUE(pc2.has_property("z"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive2.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive2.get_property<std::string>("y"));
}

TEST_F(PropertyTests, TestNetMultiEventOnlyChandedPropertySerialization) 
{
	PC_Data pc_send, pc_receive, pc_receive2;

	clan::NetGameEvent msg("msg"), msg2("msg");
	clan::NetGameEventValue val(clan::NetGameEventValue::complex), val2(clan::NetGameEventValue::complex);

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
	ASSERT_TRUE(pc_receive.has_property("other_data"));

	PropertyContainer pc = pc_receive.get_property<PropertyContainer>("other_data");
	ASSERT_TRUE(pc.has_property("w"));
	ASSERT_TRUE(pc.has_property("z"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive.get_property<std::string>("y"));

	///test 2nd msg
	ASSERT_TRUE(pc_receive2.has_property("x"));
	ASSERT_TRUE(pc_receive2.has_property("y"));
	ASSERT_TRUE(pc_receive2.has_property("other_data"));

	PropertyContainer pc2 = pc_receive2.get_property<PropertyContainer>("other_data");
	ASSERT_TRUE(pc2.has_property("w"));
	ASSERT_TRUE(pc2.has_property("z"));

	ASSERT_EQ(pc_send.get_property<uint32_t>("x"),pc_receive2.get_property<uint32_t>("x"));
	ASSERT_EQ(pc_send.get_property<std::string>("y"),pc_receive2.get_property<std::string>("y"));
}

Client*	m_clients;

void check_msg(const Message & msg)
{
	ASSERT_EQ(&msg,&m_clients[1]);

	ASSERT_EQ(456,msg.get_property<uint32_t>("id"));
	ASSERT_EQ(std::string("10"),msg.get_property<std::string>("name").get());
}

void set_msg(Client & msg)
{
	ASSERT_EQ(&msg,&m_clients[1]);
	msg.set_id(456);
	msg.set_name("10");
	msg.set_flag(0);

	ASSERT_EQ(&msg,&m_clients[1]);

	ASSERT_EQ(456,msg.get_id());
	ASSERT_EQ(456,msg.get_property<uint32_t>("id"));
	ASSERT_EQ(std::string("10"),msg.get_property<std::string>("name").get());
}

TEST_F(PropertyTests, MessagePtrDereferenceTestNoReset)
{
	m_clients =	new Client[4];

	Client* m = &m_clients[1];

	//m_clients[1]=Client();
	set_msg(*m);
	check_msg(*m);
}


TEST_F(PropertyTests, MessagePtrDereferenceTest)
{
	m_clients =	new Client[4];

	Client* m = &m_clients[1];

	m_clients[1]=Client();

	set_msg(*m);
	//check_msg(*m);
}

TEST_F(PropertyTests, NumericValueSetGetTest)
{
	PC_Data p;

	p.x=15;
	p.y="hello";

	ASSERT_EQ(15,p.get_property<uint32_t>("x"));
	ASSERT_EQ(std::string("hello"),p.get_property<std::string>("y").get());
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();

	std::getwchar();
	return ret;
}