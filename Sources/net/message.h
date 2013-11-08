#pragma once

#include "properties/property.h"
#include "properties/property_container.h"

enum MESSAGE_TYPE
{
	MSG_GENERIC=0,
	MSG_CLIENT_INPUT,
	MSG_CLIENT_AUTH,
};

#define DEF_MSG(CLASSNAME,ENUM) public: static uint32_t type(){return ENUM;} static Message * create(){return new CLASSNAME();} virtual uint32_t get_type()const{return CLASSNAME::type();}

class Message: public PropertyContainer
{
	DEF_MSG(Message,MSG_GENERIC)
public:
	Property<uint32_t> timestamp; ///time in ms

	Message()
	{
		timestamp = add_property<uint32_t>("timestamp",0);
	}

///------Message factory-------
protected:
	typedef Message * (*msg_create_func)();
	static std::map<uint32_t, msg_create_func> m_msg_create;

	
public:
	static Message * create_message(uint32_t type);

	template <class T>
	static bool register_message()
	{
		m_msg_create[T::type()]=&T::create;
		return true;
	}

	static bool register_messages();
};

/**
###############################################
############# Net messages ####################
###############################################
**/

class MSG_Client_Auth: public Message
{
	DEF_MSG(MSG_Client_Auth,MSG_CLIENT_AUTH)
public:
	Property<std::string> name;

	MSG_Client_Auth()
	{
		name = add_property<std::string>("name");
	}
};

/**
###############################################
############# Input messages ##################
###############################################
**/

enum EUSER_INPUT_KEY
{
	EUIKT_MOVE_LEFT=BIT(0),
	EUIKT_MOVE_RIGHT=BIT(1),
	EUIKT_MOVE_UP=BIT(2),
	EUIKT_MOVE_DOWN=BIT(3)
};

class MSG_Client_Input: public Message
{
	DEF_MSG(MSG_Client_Input,MSG_CLIENT_INPUT)
public:
	Property<uint32_t> keys;	  ///enumerated types
	
	MSG_Client_Input()
	{
		keys = add_property<uint32_t>("keys",0);
	}
};

