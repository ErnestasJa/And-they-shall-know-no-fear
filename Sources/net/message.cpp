#include "precomp.h"
#include "message.h"
#include "game_objects/game_objects.h"

std::map<uint32_t, Message::msg_create_func>  Message::m_msg_create;

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
	register_message<MSGS_ClientDisconnect>();
	register_message<MSGS_GameObjectAction>();
	
	///cient/server
	register_message<MSG_Query>();
	register_message<MSG_QueryResponse>();

	return true;
}

///-------------------------------------------

void MessageUtil::add_message(clan::NetGameEvent & net_event, const Message & m, bool serialize_only_changed)
{
	clan::NetGameEventValue val(clan::NetGameEventValue::complex);
	m.net_serialize(val,serialize_only_changed);

	net_event.add_argument(m.get_type());
	net_event.add_argument(val);
}

void MessageUtil::add_game_object(clan::NetGameEvent & net_event, GameObject * m, bool serialize_only_changed)
{
	clan::NetGameEventValue val(clan::NetGameEventValue::complex);
	m->net_serialize(val,serialize_only_changed);

	net_event.add_argument(MSG_GAME_OBJECT);
	net_event.add_argument(val);
}

void MessageUtil::get_message(const clan::NetGameEvent & net_event, Message & m, uint32_t index)
{
	m.net_deserialize(net_event.get_argument(index*2+1));
}

void MessageUtil::get_game_object(const clan::NetGameEvent & net_event, GameObject * m, uint32_t index)
{
	m->net_deserialize(net_event.get_argument(index*2+1));
}

uint32_t MessageUtil::get_message_count(const clan::NetGameEvent & net_event)
{
	return net_event.get_argument_count()/2;
}

///-------------------------------------------
Client::Client()
{
	m_id	= add_property<uint32_t>("id",0,EPF_ALWAYS_SEND);
	m_flags = add_property<uint32_t>("flags",0,EPF_ALWAYS_SEND);
	m_name  = add_property<std::string>("name","",EPF_ALWAYS_SEND);
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