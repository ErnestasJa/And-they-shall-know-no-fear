#include "precomp.h"
#include "common.h"
#include "message.h"
#include "user_client.h"

void ClientConnection::connect(const std::string & server,const std::string & port)
{
	m_net_client.connect(server,port);
}

void ClientConnection::disconnect()
{
	m_net_client.disconnect();
}

clan::Signal_v0 &  ClientConnection::sig_connected()
{
	return m_net_client.sig_connected();
}

clan::Signal_v0 &  ClientConnection::sig_disconnected()
{
	return m_net_client.sig_disconnected();
}

clan::Signal_v1< const clan::NetGameEvent & > & ClientConnection::sig_event_received()
{
	return m_net_client.sig_event_received();
}

void ClientConnection::send_event(const clan::NetGameEvent & e)
{
	m_net_client.send_event(e);
}