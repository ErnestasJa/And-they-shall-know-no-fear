#include "precomp.h"
#include "common.h"
#include "server.h"
#include "server_client.h"
#include "message.h"
#include "game_objects/game_object_manager.h"
#include "game_objects/game_objects.h"


bool Server::init(uint32_t max_clients, const std::string & port)
{
	register_property_types();
	Message::register_messages();
	m_gom = new GameObjectManager();
	m_gom->register_game_object<GOSprite>();
	
	m_clients = new ServerClient[max_clients];
	m_player_objects = new GOSprite * [max_clients];

	loopi(max_clients){
		m_player_objects[i] = (GOSprite*) m_gom->add_game_object(EGOT_SPRITE,i);
		m_clients[i].m_id = i;
		m_clients[i].init();
	}

	

	m_slots.connect(m_net_server.sig_client_connected(), this, &Server::on_client_connected);
	m_slots.connect(m_net_server.sig_client_disconnected(), this, &Server::on_client_disconnected);
	m_slots.connect(m_net_server.sig_event_received(), this, &Server::on_event);

	m_login_events.func_event("msg").set(this, &Server::on_auth);
	m_game_events.func_event("msg").set(this, &Server::on_game_event);

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
			m_clients[i].init();
			m_clients[i].connect(connection);
			connection->set_data("cl_ptr",&m_clients[i]);

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
	{
		clan::log_event("net_event","Client disconnected: id '%1'.",client->get_id());
		connection->set_data("cl_ptr",nullptr);
		client->disconnect();
	}
}


void Server::on_auth(const clan::NetGameEvent &e, ServerClient * client)
{
	uint32_t type = e.get_argument(0).to_uinteger();

	if(type==MSGC_AUTH)
	{
		MSGC_Auth * m = static_cast<MSGC_Auth*>(Message::create_message(type));
		m->net_deserialize(e);

		MSGS_AuthStatus msg;
		std::string str;

		if(m->name.get().size()<33&&m->name.get().size()>2)
		{
			client->set_name(m->name);
			client->set_flag(ECF_LOGGED_IN);

			clan::StringFormat fmt("Client with user name '%1' connected, id '%2'.");
			fmt.set_arg(0,client->get_name());
			fmt.set_arg(1,client->get_id());

			msg.auth_sucessful	= true;
			msg.id				= client->get_id();
			msg.msg				= fmt.get_result();

			clan::log_event("net_event",msg.msg);

			client->send_message(msg);
		}
		else
		{
			msg.auth_sucessful	= false;
			msg.msg				= "Nick name should be from 3 to 32 symbols length.";
			
			client->send_message(msg);
			client->get_connection()->disconnect();/// disconnect doesn't happen before you send another msg.
			
			clan::log_event("net_event",msg.msg);
		}
	}
	else
	{
		client->get_connection()->disconnect();
		clan::log_event("net_event", "Expected auth event from client, but client sent event of type: %1", type);
	}
}

void Server::on_game_event(const clan::NetGameEvent &e, ServerClient * user)
{
	uint32_t type = e.get_argument(0).to_uinteger();

	if(type==MSGC_INPUT)
	{
		MSGC_Input * m = static_cast<MSGC_Input*>(Message::create_message(type));
		m->net_deserialize(e);

		GOSprite * spr=m_player_objects[user->get_id()];
		spr->on_message(*m);
	}
	else
	{
		clan::log_event("net_event", "Unexpected event received from client, type: %1", type);
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
	delete m_gom;
	m_net_server.stop();
}