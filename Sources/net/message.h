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
	MSGS_GAME_OBJECT_ACTION,
	MSGS_SERVER_INFO,

	///client/server
	MSG_QUERY,	///query, query response are a bit abstract message types to avoid too many message classes
	MSG_QUERY_RESPONSE,
	MSG_CLIENT_INFO,
	MSG_GAME_OBJECT
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
############# ClientConnection messages #################
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
	Property<uint32_t> id;	  ///cliento id
	Property<uint32_t> keys;	  ///enumerated types
	
	MSGC_Input()
	{
		id = add_property<uint32_t>("id",0,EPF_ALWAYS_SEND);
		keys = add_property<uint32_t>("keys",0,EPF_ALWAYS_SEND);
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

enum EGameObjectActionType
{
	EGOAT_CREATE = 0,
	EGOAT_UPDATE,
	EGOAT_REMOVE
};

class MSGS_GameObjectAction: public Message
{
	DEF_MSG(MSGS_GameObjectAction,MSGS_GAME_OBJECT_ACTION)
public:
	Property<uint32_t> action_type;
	Property<uint32_t> guid;
	Property<uint32_t> object_type;
	Property<PropertyContainer> object_properties;

	MSGS_GameObjectAction()
	{
		action_type = add_property<uint32_t>("at");
		guid = add_property<uint32_t>("guid");
		object_type = add_property<uint32_t>("ot");
		object_properties = add_property<PropertyContainer>("op");
	}
};

class MSG_Server_Info: public Message
{
	DEF_MSG(MSG_Server_Info, MSGS_SERVER_INFO)
public:
	Property<std::string>	map_name;
	Property<uint32_t>		max_client_count;

	MSG_Server_Info()
	{
		map_name = add_property<std::string>("name");
		max_client_count = add_property<uint32_t>("max_client_count");
		
		//map name, client count
		//world server
	}
};

/**
###############################################
########## ClientConnection/Server messages #############
###############################################
**/

class Client: public Message
{
	DEF_MSG(Client,MSG_CLIENT_INFO)
public:
	Client();

	void clear_flag(uint32_t flag);
	bool check_flag(uint32_t flag);
	void set_flag(uint32_t flag);

	uint32_t get_id();
	void set_id(uint32_t id);

	void set_name(const std::string & name);
	std::string get_name();

protected:
	Property<uint32_t>		m_flags;
	Property<uint32_t>		m_id;
	Property<std::string>	m_name;
};

enum EQueryType
{
	///client asks server
	EQT_SERVER_INFO=1,

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