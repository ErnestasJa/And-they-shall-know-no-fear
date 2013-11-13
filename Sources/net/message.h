#pragma once

#include "properties/property.h"
#include "properties/property_container.h"

/**
MSG[x]_MESSAGE_NAME
x = 'C' = only sent from client to server
x = 'S' = only sent from server to client
x = ''  = sent from client to server and from server to client
**/

enum MESSAGE_TYPE
{
	MSG_GENERIC=0,

	//client
	MSGC_INPUT,
	MSGC_AUTH,

	//server 
	MSGS_AUTH_STATUS,
	MSGS_CLIENT_PUNISHMENT,
	MSGS_CREATE_GAME_OBJECT,

	///client/server
	MSG_QUERY, ///query, query response are a bit abstract message types to avoid too many message classes
	MSG_QUERY_RESPONSE,
};

#define DEF_MSG(CLASSNAME,ENUM) public: static uint32_t msg_type(){return ENUM;} static Message * create(){return new CLASSNAME();} virtual uint32_t get_type()const{return CLASSNAME::msg_type();}

class Message: public PropertyContainer
{
	DEF_MSG(Message,MSG_GENERIC)
public:
	Property<uint32_t> timestamp; ///time in ms

	Message()
	{
		timestamp = add_property<uint32_t>("ts",0);
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
		m_msg_create[T::msg_type()]=&T::create;
		return true;
	}

	static bool register_messages();
};

/**
###############################################
############# Client messages #################
###############################################
**/

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


/**
###############################################
############# Server messages #################
###############################################
**/

class MSGS_AuthStatus: public Message
{
	DEF_MSG(MSGS_AuthStatus,MSGS_AUTH_STATUS)
public:
	Property<bool> auth_sucessful;
	Property<uint32_t> id;
	Property<std::string> msg;

	MSGS_AuthStatus()
	{
		auth_sucessful = add_property<bool>("as");
		id = add_property<uint32_t>("id");
		msg = add_property<std::string>("name");
	}
};


enum EClientPunishmentType
{
	ECPT_KICK = 0,
	ECPT_BAN,
};

class MSGS_ClientPunishment: public Message
{
	DEF_MSG(MSGS_ClientPunishment,MSGS_CLIENT_PUNISHMENT)
public:
	Property<uint32_t> id, type;

	MSGS_ClientPunishment()
	{
		id = add_property<uint32_t>("id");
		type = add_property<uint32_t>("type");
	}
};

class MSGS_CreateGameObject: public Message
{
	DEF_MSG(MSGS_CreateGameObject,MSGS_CREATE_GAME_OBJECT)
public:
	Property<uint32_t> guid;
	Property<uint32_t> obj_type;

	MSGS_CreateGameObject()
	{
		guid = add_property<uint32_t>("guid");
		obj_type = add_property<uint32_t>("type");
	}
};

/**
###############################################
########## Client/Server messages #############
###############################################
**/

enum EQueryType
{
	///client asks server
	EQT_MAP_INFO=1,
	EQT_PLAYER_INFO,

	//server asks client

};

class MSG_Query: public Message
{
	DEF_MSG(MSG_Query, MSG_QUERY)
public:
	Property<uint32_t> query_type;

	MSG_Query()
	{
		query_type = add_property<uint32_t>("qt");
	}
};

class MSG_QueryResponse: public Message
{
	DEF_MSG(MSG_QueryResponse, MSG_QUERY_RESPONSE)
public:
	Property<uint32_t> query_type;

	MSG_QueryResponse()
	{
		query_type = add_property<uint32_t>("qt");
	}
};
