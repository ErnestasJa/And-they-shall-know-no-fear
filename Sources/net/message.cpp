#include "precomp.h"
#include "message.h"
#include "game_objects/game_objects.h"

std::map<uint32_t, Message::msg_create_func>  Message::m_msg_create;

///factory
Message * Message::create_message(uint32_t type)
{
	auto it = m_msg_create.find(type);

	if(it != m_msg_create.end())
		return it->second();

	throw clan::Exception("Cannot create object of type " + clan::StringHelp::uint_to_text(type));
}

bool Message::register_messages()
{
	register_message<Message>();

	///client
	register_message<MSGC_Input>();
	register_message<MSGC_Auth>();

	//server
	register_message<MSGS_AuthStatus>();
	register_message<MSGS_ClientPunishment>();
	register_message<MSGS_GameObjectAction>();
	
	///cient/server
	register_message<MSG_Query>();
	register_message<MSG_QueryResponse>();

	return true;
}

/// Client
Client::Client()
{
	m_id = add_property<uint32_t>("id");
	m_flags = add_property<uint32_t>("flags",0);
	m_name  = add_property<std::string>("name");
}

void Client::clear_flag(uint32_t flag)
{
	m_flags = m_flags & ~flag;
}

bool Client::check_flag(uint32_t flag)
{
	return ((m_flags & flag) == flag);
}

void Client::set_flag(uint32_t flag)
{
	m_flags = m_flags | flag;
}

uint32_t Client::get_id()
{
	return m_id;
}

void Client::set_id(uint32_t id)
{
	m_id = id;
}

std::string Client::get_name()
{
	return m_name;
}

void Client::set_name(const std::string & name)
{
	m_name = name;
}