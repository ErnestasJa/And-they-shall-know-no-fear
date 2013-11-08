#include "precomp.h"
#include "common.h"
#include "message.h"
#include "client.h"

void Client::connect(const std::string & server,const std::string & port)
{
	m_net_client.connect(server,port);
}

clan::Signal_v0 &  Client::sig_connected()
{
	return m_net_client.sig_connected();
}

clan::Signal_v0 &  Client::sig_disconnected()
{
	return m_net_client.sig_disconnected();
}

clan::Signal_v1< const clan::NetGameEvent & > & Client::sig_event_received()
{
	return m_net_client.sig_event_received();
}

void Client::clear_flag(uint32_t flag)
{
	m_flags &= ~flag;
}

bool Client::check_flag(uint32_t flag)
{
	return ((m_flags & flag) == flag);
}

void Client::set_flag(uint32_t flag)
{
	m_flags |= flag;
}


const std::string & Client::get_name()
{
	return m_name;
}

void Client::set_name(const std::string & name)
{
	m_name = name;
}

void Client::send_message(const Message & msg)
{
	clan::NetGameEvent e("msg");
	e.add_argument(msg.get_type());
	msg.net_serialize(e);
	m_net_client.send_event(e);
}