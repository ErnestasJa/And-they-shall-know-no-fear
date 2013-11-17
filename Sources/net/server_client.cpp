#include "precomp.h"
#include "common.h"
#include "message.h"
#include "server_client.h"

ServerClientConnection::ServerClientConnection()
{
}

ServerClientConnection::~ServerClientConnection()
{
	
}

void ServerClientConnection::init(Client * client)
{
	m_connection = nullptr;
	m_client = client;
}

Client * ServerClientConnection::get_client()
{
	return m_client;
}

void ServerClientConnection::send_message(const Message & msg, bool only_changed)
{
	clan::NetGameEventValue val(clan::NetGameEventValue::complex);
	msg.net_serialize(val);

	clan::NetGameEvent e("msg");
	e.add_argument(msg.get_type());
	e.add_argument(val);
	m_connection->send_event(e);
}

clan::NetGameConnection * ServerClientConnection::get_connection()
{
	return m_connection;
}

void ServerClientConnection::connect(clan::NetGameConnection * connection)
{
	m_connection = connection;
}

bool ServerClientConnection::is_connected()
{
	return m_connection!=nullptr;
}

void ServerClientConnection::disconnect()
{
	m_client = nullptr;
	m_connection = nullptr;
}

ServerClientConnection * ServerClientConnection::get_client(clan::NetGameConnection * connection)
{
	return reinterpret_cast<ServerClientConnection*>(connection->get_data("cl_ptr"));
}
