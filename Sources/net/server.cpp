#include "precomp.h"
#include "common.h"
#include "server.h"
#include "server_client.h"
#include "message.h"


bool Server::init(uint32_t max_clients, const std::string & port)
{
	Message::register_messages();

	m_clients = new ServerClient[max_clients];
	
	loopi(max_clients)
		m_clients[i].init(i);

	m_slots.connect(m_net_server.sig_client_connected(), this, &Server::on_client_connected);
	m_slots.connect(m_net_server.sig_client_disconnected(), this, &Server::on_client_disconnected);
	m_slots.connect(m_net_server.sig_event_received(), this, &Server::on_event);

	m_login_events.func_event("msg").set(this, &Server::on_auth);
	//m_game_events.func_event("query_game_info").set(this, &Server::on_event_query_game_info);

	m_net_server.start(port);

	m_game_time = clan::GameTime();

	

	return true;
}

bool Server::run()
{
	//m_game_time.update();
	return true;
}

ServerClient & Server::get_client(uint32_t id)
{
	return m_clients[id];
}

uint32_t Server::get_max_clients()
{
	return m_max_clients;
}

void Server::on_client_connected(clan::NetGameConnection *connection)
{
	loopi(m_max_clients)
	{
		if(!m_clients[i].is_connected())
		{
			m_clients[i].connect(connection);
			clan::log_event("net_event","User connected, id '%1'.",i);
			return;
		}
	}

	clan::log_event("net_event","User disconnected: too many clients already connected.");
	connection->disconnect();
}

void Server::on_client_disconnected(clan::NetGameConnection *connection, const std::string &message)
{
	ServerClient* client = ServerClient::get_client(connection);

	if(client)
		client->disconnect();
}


void Server::on_auth(const clan::NetGameEvent &e, ServerClient * client)
{
	uint32_t type = e.get_argument(0).to_uinteger();

	if(type==MSG_CLIENT_AUTH)
	{
		MSG_Client_Auth * m = static_cast<MSG_Client_Auth*>(Message::create_message(type));
		m->net_deserialize(e);
		
		if(m->name.get().size()<33&&m->name.get().size()>2){
			client->set_name(m->name);
			client->init(client->get_id());
			client->set_flag(ECF_LOGGED_IN);
			clan::log_event("net_event","Client with user name '%1' connected, id '%2'.",client->get_name(),client->get_id());
		}
		else
		{
			client->get_connection()->disconnect();
			client->disconnect();
			clan::log_event("net_event","Nick name should be from 3 to 32 symbols length");
		}
		
	}
	else
	{
		clan::log_event("net_event", "Expected auth event from client, but client sent event of type: %1", type);
	}


}

void Server::on_event(clan::NetGameConnection *connection, const clan::NetGameEvent &e)
{
	clan::log_event("events", "Client sent event: %1", e.to_string());

	ServerClient *client = ServerClient::get_client(connection);
	if(client)
	{
		bool handled_event = false;

		if (!client->check_flag(ECF_LOGGED_IN))	// User has not logged in, so route events to login dispatcher
		{
			// Route login events
			handled_event = m_login_events.dispatch(e, client);
		}
		else
		{
			// Route game events
			handled_event = m_game_events.dispatch(e, client);
		}

		if (!handled_event)
		{
			// We received an event which we didn't hook up
			clan::log_event("events", "Unhandled event: %1", e.to_string());
		}
	}
}

void Server::exit()
{
	m_net_server.stop();
}