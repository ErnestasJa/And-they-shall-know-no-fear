#include "precomp.h"
#include "common.h"
#include "message.h"
#include "server_client.h"

ServerClient::ServerClient()
{
}

ServerClient::~ServerClient()
{
	
}

void ServerClient::init()
{
	m_connection = nullptr;
	m_flags = 0;
}

const std::string & ServerClient::get_name()
{
	return m_name;
}

void ServerClient::set_name(const std::string & name)
{
	m_name = name;
}

uint32_t ServerClient::get_id()
{
	return m_id;
}

void ServerClient::clear_flag(uint32_t flag)
{
	m_flags &= ~flag;
}

bool ServerClient::check_flag(uint32_t flag)
{
	return ((m_flags & flag) == flag);
}

void ServerClient::set_flag(uint32_t flag)
{
	m_flags |= flag;
}

void ServerClient::send_message(const Message & msg)
{
	clan::NetGameEvent e("msg");
	e.add_argument(msg.get_type());
	msg.net_serialize(e);
	m_connection->send_event(e);
}

clan::NetGameConnection * ServerClient::get_connection()
{
	return m_connection;
}

void ServerClient::connect(clan::NetGameConnection * connection)
{
	m_connection = connection;
}

bool ServerClient::is_connected()
{
	return m_connection!=nullptr;
}

void ServerClient::disconnect()
{
	m_connection = nullptr;
}

ServerClient * ServerClient::get_client(clan::NetGameConnection * connection)
{
	return reinterpret_cast<ServerClient*>(connection->get_data("cl_ptr"));
}
