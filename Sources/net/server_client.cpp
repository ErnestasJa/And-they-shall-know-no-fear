#include "precomp.h"
#include "common.h"
#include "server_client.h"

ServerClient::ServerClient()
{

}

ServerClient::~ServerClient()
{
	init(0);
}

void ServerClient::init(uint32_t id)
{
	m_id = id;
	m_connection = nullptr;
	m_flags = 0;
}

void ServerClient::connect(clan::NetGameConnection * connection)
{
	m_connection = connection;
	m_connection->set_data("cl_ptr",this);
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

clan::NetGameConnection * ServerClient::get_connection()
{
	return m_connection;
}

bool ServerClient::is_connected()
{
	return m_connection!=nullptr;
}

void ServerClient::disconnect()
{
	m_connection->set_data("cl_ptr",nullptr);
	m_connection = nullptr;
}

ServerClient * ServerClient::get_client(clan::NetGameConnection * connection)
{
	return reinterpret_cast<ServerClient*>(connection->get_data("cl_ptr"));
}
