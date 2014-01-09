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

	m_game_time = clan::GameTime(33,33);

	m_gom = new GameObjectManager();
	m_gom->func_on_add_game_object().set(this,&Server::on_add_game_object);
	m_gom->func_on_update_game_object().set(this,&Server::on_update_game_object);
	m_gom->func_on_remove_game_object().set(this,&Server::on_remove_game_object);

	m_max_clients = max_clients;
	m_current_guid = max_clients;
	
	m_client_cons = new ServerClientConnection[m_max_clients];
	m_clients =		new Client[m_max_clients];
	m_player_objects = new Player * [m_max_clients];

	m_slots.connect(m_net_server.sig_client_connected(), this, &Server::on_client_connected);
	m_slots.connect(m_net_server.sig_client_disconnected(), this, &Server::on_client_disconnected);
	m_slots.connect(m_net_server.sig_event_received(), this, &Server::on_event);

	m_auth_events.func_event("amsg").set(this, &Server::on_auth);
	m_game_events.func_event("gmsg").set(this, &Server::on_game_event);
	m_net_events.func_event ("nmsg").set(this, &Server::on_net_event);

	m_net_server.start(port);

	return true;
}

void Server::send_game_objects(ServerClientConnection * cl, bool sync_only_changed_props)
{
	clan::NetGameEvent e("gmsg");
	MSGS_GameObjectAction cc;

	for(auto it = m_gom->get_game_objects().begin(); it != m_gom->get_game_objects().end(); it++)
	{
		if((*it)->is_alive())
		{
			cc.action_type = EGOAT_CREATE;
			cc.guid = (*it)->get_guid();
			cc.object_type = (*it)->get_type();
			MessageUtil::add_message(e,cc);
		}
	}

	for(auto it = m_gom->get_tmp_game_objects().begin(); it != m_gom->get_tmp_game_objects().end(); it++)
	{
		if((*it)->is_alive())
		{
			cc.action_type = EGOAT_CREATE;
			cc.guid = (*it)->get_guid();
			cc.object_type = (*it)->get_type();
			MessageUtil::add_message(e,cc);
		}
	}
	
	cl->send_event(e);
}

void Server::sync_game_objects(bool sync_only_changed_props)
{
	clan::NetGameEvent e("gosmsg");

	for(auto it = m_gom->get_game_objects().begin(); it != m_gom->get_game_objects().end(); it++)
	{
		if((*it)->is_alive())
		MessageUtil::add_game_object(e,(*it),false);
	}

	for(auto it = m_gom->get_tmp_game_objects().begin(); it != m_gom->get_tmp_game_objects().end(); it++)
	{
		if((*it)->is_alive())
		MessageUtil::add_game_object(e,(*it),false);
	}
	
	m_net_server.send_event(e);
}



void Server::on_add_game_object(GameObject * obj)
{
	MSGS_GameObjectAction msg;
	msg.action_type = EGOAT_CREATE;
	msg.guid = obj->get_guid();
	msg.object_type = obj->get_type(); /// realiai tipas nereikalingas

	clan::NetGameEvent ev("gmsg");
	MessageUtil::add_message(ev,msg);
	m_net_server.send_event(ev);
}

void Server::on_remove_game_object(GameObject * obj)
{
	MSGS_GameObjectAction msg;
	msg.action_type = EGOAT_REMOVE;
	msg.guid = obj->get_guid();
	msg.object_type = obj->get_type(); /// realiai tipas nereikalingas

	clan::NetGameEvent ev("gmsg");
	MessageUtil::add_message(ev,msg);
	m_net_server.send_event(ev);
}

void Server::on_update_game_object(GameObject * obj)
{
	if(obj->get_type()==EGOT_THROWABLE_OBJECT)
	{
		ThrowableObject * o = static_cast<ThrowableObject *>(obj);

		if(o->get_spawn_time()+3000<m_game_time.get_current_time_ms())
		{
			o->set_is_alive(false);
		}
	}
	if(obj->get_type() == EGOT_PLAYER)
	{
		if(obj->get_property<uint32_t>("life")==0)
			obj->set_is_alive(false);
	}
}

bool Server::run()
{
	
	m_game_time.update();

	if(m_gom)
	{
		m_gom->update_game_objects(m_game_time);
		m_gom->collide_game_objects(m_game_time);
		m_gom->remove_not_alive_objects();

		sync_game_objects();
	}


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
		
		m_gom->remove_game_object(id);		

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

		clan::NetGameEvent ev("amsg");

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

			MessageUtil::add_message(ev,msg);
			con->send_event(ev);
		}
		else
		{
			msg.auth_sucessful	= false;
			msg.msg				= "Nick name should be from 3 to 32 symbols length.";
			
			MessageUtil::add_message(ev,msg);
			con->send_event(ev);
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
	///process every message in event
	Client * client = con->get_client();
	for(uint32_t i = 0; i < MessageUtil::get_message_count(e); i++)
	{
		uint32_t type = MessageUtil::get_message_type(e,i);

		if(type==MSGC_INPUT)
		{
			clan::log_event("net_event", "Move event");
			MSGC_Input m;
			MessageUtil::get_message(e,m,i);

			Player * p = static_cast<Player *>(m_gom->find_game_object_by_guid(client->get_id()));

			if(p!=nullptr)
			{
				p->on_message(m);

				if(m.keys&EUIKT_ATTACK)
				{
					if(p->get_next_attack_time()<=m_game_time.get_current_time_ms())
					{
						///spawn rock, shoot rock
						ThrowableObject * obj = static_cast<ThrowableObject *>(m_gom->add_game_object(EGOT_THROWABLE_OBJECT, m_current_guid));
						obj->init(m_game_time.get_current_time_ms(),p->get_guid());

						m_current_guid ++;

						clan::vec2f vel;
						vel.x = (m.keys & EUIKT_MOVE_LEFT ? - 1 : (m.keys & EUIKT_MOVE_RIGHT ? 1 : 0 ) );
						vel.y = (m.keys & EUIKT_MOVE_UP ? - 1 : (m.keys & EUIKT_MOVE_DOWN ? 1 : 0 ) );
						vel = vel.normalize();
						vel *= 128.0f;

						clan::vec2f off;
						off.x = 26;
						off.y = 28;

						obj->get_vel().set(vel);
						obj->get_pos().set(m_gom->find_game_object_by_guid(client->get_id())->get_pos().get()+off);
						p->set_next_attack_time(m_game_time.get_current_time_ms() + 1000);
					}
				}
			}
		}
		else ///maybe game object manager might handle this message
			m_gom->on_net_event(e);
	}
}

void Server::on_net_event(const clan::NetGameEvent &e, ServerClientConnection * con)
{
	Client * client = con->get_client();
	uint32_t type = e.get_argument(0).to_uinteger();

	if(type==MSG_QUERY)
	{
		MSG_Query q;
		MSG_Server_Info si;

		q.net_deserialize(e.get_argument(1));

		if(q.query_type==EQT_SERVER_INFO)
		{
			clan::NetGameEvent si_ev("nmsg");
			clan::NetGameEvent ci_ev("nmsg");
			

			si.map_name = "Level/next_level.map";
			si.max_client_count = m_max_clients;

			///nusiunciam siam klientui serverio informacija
			MessageUtil::add_message(si_ev,si);
			con->send_event(si_ev);

			///nusiunciam visiem sio kliento informacija
			MessageUtil::add_message(ci_ev,*client);
			m_net_server.send_event(ci_ev);

			///nusiunciam siam klientui kitu klientu informacija
			loopi(m_max_clients)
			{
				if(client->get_id() != i && m_client_cons[i].is_connected())
				{
					clan::NetGameEvent ci_ev2("nmsg");
					MessageUtil::add_message(ci_ev2,m_clients[i]);
					con->send_event(ci_ev2);
				}
			}
		}
	}
	else if(type==MSGC_READY)
	{
		send_game_objects(con);
		///sukuriam sio kliento zaidimo objekta
		m_player_objects[client->get_id()]=static_cast<Player*>(m_gom->add_game_object(EGOT_PLAYER,client->get_id()));
		m_player_objects[client->get_id()]->get_pos().set(clan::vec2(std::rand()%1216,std::rand()%656));
		m_player_objects[client->get_id()]->get_property<std::string>("name").set(client->get_name());
	}
}

void Server::on_event(clan::NetGameConnection *connection, const clan::NetGameEvent &e)
{
	ServerClientConnection * con = ServerClientConnection::get_client(connection);
	Client* client = con->get_client();

	if(con)
	{
		bool handled_event = false;

		if (!client->check_flag(ECF_LOGGED_IN)&&e.get_name()=="amsg")	// User has not logged in, so route events to login dispatcher
		{
			handled_event = m_auth_events.dispatch(e, con);
		}
		else 
		{
			if(e.get_name()=="gmsg")
				handled_event = m_game_events.dispatch(e, con);
			else if(e.get_name()=="nmsg")
				handled_event = m_net_events.dispatch(e, con);
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