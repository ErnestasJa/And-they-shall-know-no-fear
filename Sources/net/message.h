#pragma once

#include "properties/property.h"
#include "properties/property_container.h"

enum MESSAGE_TYPE
{
	MSG_GENERIC=0,
	MSGC_INPUT,
	MSGC_AUTH,
	MSGS_AUTH_STATUS,

	///Game
	MSGS_CREATE_GAME_OBJECT
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

class MSGC_Auth: public Message
{
	DEF_MSG(MSGC_Auth,MSGC_AUTH)
public:
	Property<std::string> name;

	MSGC_Auth()
	{
		name = add_property<std::string>("name");
	}
};

class MSGS_Auth_Status: public Message
{
	DEF_MSG(MSGS_Auth_Status,MSGS_AUTH_STATUS)
public:
	Property<bool> auth_sucessful;
	Property<uint32_t> id;

	MSGS_Auth_Status()
	{
		auth_sucessful = add_property<bool>("as");
		id = add_property<uint32_t>("id");
	}
};

/**
###############################################
############# Game messages ####################
###############################################
**/

class MSGS_Create_Game_Object: public Message
{
	DEF_MSG(MSGS_Create_Game_Object,MSGS_CREATE_GAME_OBJECT)
public:
	Property<uint32_t> guid;
	Property<uint32_t> obj_type;

	MSGS_Create_Game_Object()
	{
		guid = add_property<uint32_t>("guid");
		obj_type = add_property<uint32_t>("type");
	}
};

enum EUSER_INPUT_KEY
{
	EUIKT_MOVE_LEFT=BIT(0),
	EUIKT_MOVE_RIGHT=BIT(1),
	EUIKT_MOVE_UP=BIT(2),
	EUIKT_MOVE_DOWN=BIT(3)
};

class MSGC_Input: public Message
{
	DEF_MSG(MSGC_Input,MSGC_INPUT)
public:
	Property<uint32_t> keys;	  ///enumerated types
	
	MSGC_Input()
	{
		keys = add_property<uint32_t>("keys",0);
	}
};

