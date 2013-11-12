#include "precomp.h"
#include "message.h"

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
	register_message<MSGC_Input>();
	register_message<MSGC_Auth>();
	register_message<MSGS_Auth_Status>();
	register_message<MSGS_Create_Game_Object>();
	

	return true;
}