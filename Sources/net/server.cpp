#include "precomp.h"
#include "common.h"
#include "server.h"
#include "server_client.h"
#include "message.h"
#include "game_objects/game_object_manager.h"
#include "game_objects/game_objects.h"


bool Server::init(uint32_t max_clients, const std::string & port)
{
	PropertyContainer::register_properties();
	Message::register_messages();

	m_gom = new GameObjectManager();

	m_max_clients = max_clients;
	
	m_client_cons = new ServerClientConnection[m_max_clients];
	m_clients =		new Client[m_max_clients];
	m_player_objects = new GOSprite * [m_max_clients];

	loopi(m_max_clients)
	{
		//m_player_objects[i] = (GOSprite*) m_gom->add_game_object(EGOT_SPRITE,i);
		m_client_cons[i].init(&m_clients[i]);
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

void Server::create_all_game_objects(ServerClientConnection * client)
{
	
}

bool Server::run()
{
	m_game_time.update();

	if(m_gom)
		m_gom->update_game_objects(m_game_time);

	return true;
}

void Server::on_client_connected(clan::NetGameConnection *connection)
{
	loopi(m_max_clients)
	{
		if(!m_client_cons[i].is_connected())
		{
			m_clients[i].set_id(i);
			m_client_cons[i].init(&m_clients[i]);
			m_client_cons[i].connect(connection);
			connection->set_data("cl_ptr",&m_client_cons[i]);

			clan::log_event("net_event","User connected, id '%1'.",i);
			return;
		}
	}

	clan::log_event("net_event","User disconnected: too many clients already connected.");
	connection->disconnect();
}

void Server::on_client_disconnected(clan::NetGameConnection *connection, const std::string &message)
{
	ServerClientConnection* con = ServerClientConnection::get_client(connection);
	Client* client = con->get_client();

	clan::log_event("net_event","Client disconnected: id '%1'.",client->get_id());
	if(con)
	{
		connection->set_data("cl_ptr",nullptr);
		con->disconnect();

		int32_t id = client->get_id();
		
		MSGS_GameObjectAction msg;
		msg.action_type = EGOAT_REMOVE;
		msg.guid = id;
		msg.object_type = EGOT_SPRITE; /// realiai tipas nereikalingas

		send_message(msg);

		m_clients[id] = Client(); ///reset client data
	}
}


void Server::on_auth(const clan::NetGameEvent &e, ServerClientConnection * con)
{
	uint32_t type = e.get_argument(0).to_uinteger();

	if(type==MSGC_AUTH)
	{
		MSGC_Auth m;
		m.net_deserialize(e.get_argument(1));

		MSGS_AuthStatus msg;
		std::string str;

		if(m.name.get().size()<33&&m.name.get().size()>2)
		{
			Client * client = con->get_client();
			client->set_name(m.name);
			client->set_flag(ECF_LOGGED_IN);

			clan::StringFormat fmt("Client with user name '%1' connected, id '%2'.");
			fmt.set_arg(1,con->get_client()->get_name());
			fmt.set_arg(2,con->get_client()->get_id());

			msg.auth_sucessful	= true;
			msg.id				= con->get_client()->get_id();
			msg.msg				= fmt.get_result();

			clan::log_event("net_event",msg.msg);

			con->send_message(msg);
		}
		else
		{
			msg.auth_sucessful	= false;
			msg.msg				= "Nick name should be from 3 to 32 symbols length.";
			
			con->send_message(msg);
			con->get_connection()->disconnect();/// disconnect doesn't happen before you send another msg.
			
			clan::log_event("net_event",msg.msg);
		}
	}
	else
	{
		con->get_connection()->disconnect();
		clan::log_event("net_event", "Expected auth event from client, but client sent event of type: %1", type);
	}
}

void Server::on_game_event(const clan::NetGameEvent &e, ServerClientConnection * con)
{
	Client * client = con->get_client();
	uint32_t type = e.get_argument(0).to_uinteger();

	if(type==MSGC_INPUT)
	{
		MSGC_Input m;
		m.net_deserialize(e.get_argument(1));
		//m.id = client->get_id();

		GOSprite * spr=m_player_objects[client->get_id()];
		spr->on_message(m);
		
		loopi(m_max_clients)
		{
			if(m_client_cons[i].is_connected() && i!=client->get_id() && client->check_flag(ECF_LOGGED_IN))
				m_client_cons[i].send_message(m);
		}
	}
	else if(type==MSG_QUERY)
	{
		MSG_Query q;
		MSG_Server_Info si;

		q.net_deserialize(e.get_argument(1));

		if(q.query_type==EQT_SERVER_INFO)
		{
			si.map_name = "Level/Level.map";
			si.max_client_count = m_max_clients;

			con->send_message(si);
			send_message(*client);

			loopi(m_max_clients && client->get_id() != i && m_client_cons[i].is_connected())
			{
				con->send_message(m_clients[i]);
			}
			
			m_player_objects[client->get_id()]=static_cast<GOSprite*>(m_gom->add_game_object(EGOT_SPRITE,client->get_id()));

			MSGS_GameObjectAction cc;
			cc.action_type = EGOAT_CREATE;
			cc.guid = client->get_id();
			cc.object_type = EGOT_SPRITE;
			cc.object_properties = *m_player_objects[client->get_id()];

			send_message(cc, ECF_LOGGED_IN);

			loopi(m_max_clients)
			{
				if(i != client->get_id() && m_client_cons[i].is_connected() && m_clients[i].check_flag(ECF_LOGGED_IN))
				{
					MSGS_GameObjectAction c;
					c.action_type = EGOAT_CREATE;
					c.guid = i;
					c.object_type = EGOT_SPRITE;
					c.object_properties = *m_player_objects[i];
					
					con->send_message(c);
				}
			}
		}
	}
	else
	{
		clan::log_event("net_event", "Unexpected event received from client, type: %1", type);
	}
}

void Server::send_message(const Message & msg)
{
	clan::NetGameEventValue val(clan::NetGameEventValue::complex);
	msg.net_serialize(val);

	clan::NetGameEvent e("msg");
	e.add_argument(msg.get_type());
	e.add_argument(val);
	m_net_server.send_event(e);
}

void Server::send_message(const Message & msg, uint32_t client_flags)
{
	clan::NetGameEventValue val(clan::NetGameEventValue::complex);
	msg.net_serialize(val);

	clan::NetGameEvent e("msg");
	e.add_argument(msg.get_type());
	e.add_argument(val);

	loopi(m_max_clients)
	{
		if(m_client_cons[i].is_connected() && m_clients[i].check_flag(client_flags))
		{
			m_client_cons[i].send_event(e);
		}
	}
}

void Server::on_event(clan::NetGameConnection *connection, const clan::NetGameEvent &e)
{
	clan::log_event("events", "Client sent event: %1", e.to_string());

	ServerClientConnection * con = ServerClientConnection::get_client(connection);
	Client* client = con->get_client();

	if(con)
	{
		bool handled_event = false;

		if (!client->check_flag(ECF_LOGGED_IN))	// User has not logged in, so route events to login dispatcher
		{
			// Route login events
			handled_event = m_login_events.dispatch(e, con);
		}
		else
		{
			// Route game events
			handled_event = m_game_events.dispatch(e, con);
		}

		if (!handled_event)
		{
			clan::log_event("events", "Unhandled event: %1", e.to_string());
		}
	}
}

void Server::exit()
{
	delete m_gom;
	m_net_server.stop();
}

ServerClientConnection & Server::get_client(uint32_t id)
{
	return m_client_cons[id];
}

uint32_t Server::get_max_clients()
{
	return m_max_clients;
}