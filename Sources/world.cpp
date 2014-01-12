#include "precomp.h"
#include "world.h"
#include "tile_chunk.h"
#include "net\user_client.h"
#include "game_objects\player.h"


World::World(clan::DisplayWindow &display_window)
{
	clan::SetupNetwork setup_network;
	clan::ConsoleLogger logger;

	m_window = display_window;
	m_game_time = clan::GameTime(60,60);

	m_client_con = new ClientConnection();

	m_client	= nullptr;
	m_gom		= nullptr;
	m_player	= nullptr;

	m_max_clients = 0;
	m_client_id = 0;
}

World::~World()
{

}

void World::init_level(const std::string & level)
{
	m_tile_map = TileMap(m_canvas);
	m_tile_map.load(level);
	m_gom = new GameObjectManager(m_tile_map);
	m_gom->preload(m_canvas,m_resources);
}

bool World::init()
{
	Message::register_messages();
	PropertyContainer::register_properties();

	m_canvas = clan::Canvas(m_window);

	// Setup resources
	clan::XMLResourceDocument doc("resources.xml");
	m_resources =   clan::XMLResourceManager::create(doc);
	m_run = true;

	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &World::on_key_up);
	m_key_down = m_window.get_ic().get_keyboard().sig_key_down().connect(this, &World::on_key_down);
	
	m_net_slots.connect(m_client_con->sig_connected(),this, &World::on_connected);
	m_net_slots.connect(m_client_con->sig_disconnected(),this, &World::on_disconnected);
	m_net_slots.connect(m_client_con->sig_event_received(),this, &World::on_event);

	m_auth_events.func_event("amsg").set(this, &World::on_auth_event);
	m_game_events.func_event("gmsg").set(this, &World::on_game_event);
	m_game_obj_sync_events.func_event("gosmsg").set(this, &World::on_game_object_sync_event);
	m_net_events.func_event ("nmsg").set(this, &World::on_net_event);

	m_game_event = new clan::NetGameEvent("gmsg");

	clan::log_event("system", "ClientConnection trying to connect to %1:%2 as %3",m_server_ip,m_server_port,m_client_name);
	m_client_con->connect(m_server_ip, m_server_port);
	
	return true;
}

void World::set_info(const ConnectionInfo & info)
{
	m_server_ip = info.ip;
	m_server_port = info.port; 
	m_server_pass = info.pass; 
	m_client_name = info.name;
}

void World::on_connected()
{
	clan::NetGameEvent e("amsg");

	msg_auth.name = m_client_name;
	msg_auth.timestamp = m_game_time.get_current_time_ms();

	MessageUtil::add_message(e,msg_auth);

	m_client_con->send_event(e);

	clan::log_event("net_event","Connected to server.");
}

void World::on_event(const clan::NetGameEvent & e)
{
	if(this->m_client_con)
	{
		bool handled_event = false;

		if (e.get_name()=="amsg")	// User has not logged in, so route events to login dispatcher
		{
			handled_event = m_auth_events.dispatch(e);
		}
		else 
		{
			if(e.get_name()=="gmsg")
				handled_event = m_game_events.dispatch(e);
			else if(e.get_name()=="nmsg")
				handled_event = m_net_events.dispatch(e);
			else if(e.get_name()=="gosmsg")
				handled_event = m_game_obj_sync_events.dispatch(e);
		}

		if (!handled_event)
		{
			clan::log_event("events", "Unhandled event: %1", e.to_string());
		}
	}
}

void World::on_auth_event(const clan::NetGameEvent & e)
{
	uint32_t type = e.get_argument(0).to_uinteger();
	clan::log_event("net_event","Got message from server type='%1' msg='%2'.",type,e.to_string());

	if(type==MSGS_AUTH_STATUS)
	{
		MSGS_AuthStatus m;
		m.net_deserialize(e.get_argument(1));

		if(m.auth_sucessful)
		{
			m_client_id = m.id;
			msg.id		= m.id;

			clan::NetGameEvent e("nmsg");
			MSG_Query q;
			q.query_type = EQT_SERVER_INFO;
			MessageUtil::add_message(e,q);
			m_client_con->send_event(e);
		}
		else
		{
			clan::log_event("net_event",m.msg);
			m_client_con->disconnect();
		}
	}
}

void World::on_game_event(const clan::NetGameEvent & e)
{
	if(!m_gom)
	{
		throw clan::Exception("Got game event before could create game object manager");
	}

	///process every message in event
	for(uint32_t i = 0; i < MessageUtil::get_message_count(e); i++)
	{
		uint32_t type = MessageUtil::get_message_type(e,i);

		if(type==MSGS_GAME_OBJECT_ACTION)
		{
			MSGS_GameObjectAction m;
			MessageUtil::get_message(e,m,i);

			if(m.action_type==EGOAT_CREATE)
			{
				GameObject * obj = m_gom->add_game_object(m.object_type,m.guid);

				if(m.object_type==EGOT_PLAYER)
				{
					Player * pobj = static_cast<Player*>(obj);
					pobj->load(m_canvas,m_resources);

					if( m.guid == m_client->get_id() )
						m_player = pobj;

					m_players[ m.guid ] = pobj;
				}
				else if(m.object_type==EGOT_THROWABLE_OBJECT)
				{
					ThrowableObject * tobj = static_cast<ThrowableObject*>(obj);
					tobj->load(m_canvas,m_resources);
				}
				
			}
			else if(m.action_type==EGOAT_REMOVE)
			{
				m_gom->remove_game_object(m.guid);

				if(m.guid==m_client->get_id())
				{
					m_player = nullptr;
				}
			}
		}
		else ///maybe game object manager might handle this message
			m_gom->on_net_event(e);
	}
}

void World::on_game_object_sync_event(const clan::NetGameEvent & e)
{
	if(m_gom)
	for(uint32_t i = 0; i < MessageUtil::get_game_object_count(e); i++)
	{
		uint32_t type = MessageUtil::get_game_object_type(e,i);
		uint32_t guid = MessageUtil::get_game_object_guid(e,i);

		GameObject * go = m_gom->find_game_object_by_guid(guid);

		if(go){
			MessageUtil::get_game_object(e,go,i);
		}
		else
		{
			clan::log_event("net_event","Object[%1] not found.",guid);
		}
	}
}

void World::on_net_event(const clan::NetGameEvent & e)
{
	uint32_t type = e.get_argument(0).to_uinteger();
	clan::log_event("net_event","Got message from server type='%1' msg='%2'.",type,e.to_string());

	if(type==MSGS_SERVER_INFO)
	{
		clan::NetGameEvent es("nmsg");
		MSGC_Ready mr;
		MSG_Server_Info msi;

		msi.net_deserialize(e.get_argument(1));
		
		m_max_clients = msi.max_client_count;
		init_level(msi.get_property<std::string>("name"));

		m_clients = new Client[m_max_clients];
		m_players = new Player * [m_max_clients];

		MessageUtil::add_message(es,mr);
		m_client_con->send_event(es);
	}
	else if(type==MSG_CLIENT_INFO)
	{
		Client m;
		m.net_deserialize(e.get_argument(1));

		clan::StringFormat fmt("Invalid client id: '%1'.");
		fmt.set_arg(1,m.get_id());

		if(m.get_id()<0||m.get_id()>=m_max_clients)
			throw clan::Exception(fmt.get_result());

		m_clients[m.get_id()].net_deserialize(e.get_argument(1));

		if(m.get_id()==m_client_id)
			m_client = &m_clients[m_client_id];

		clan::log_event("net_event","Got user info: name='%1', id='%2'", m.get_name(), m.get_id());
	}
}

void World::on_disconnected()
{
	clan::log_event("net_event","Disconnected from server.");
	m_run = false;
}

bool World::run()
{
	if(m_run)
	{
		if(m_gom)
		{
			m_game_time.update();
			m_canvas.clear();
			
			m_tile_map.render(m_pos,0,LAYER_COUNT-1);
			m_gom->update_game_objects(m_game_time);
			m_gom->render_game_objects(m_canvas);
			m_tile_map.render(m_pos,LAYER_COUNT-1,LAYER_COUNT);

			if(MessageUtil::get_message_count(*m_game_event)>0)
			{
				m_client_con->send_event(*m_game_event);
				clan::NetGameEvent * tmp = m_game_event;
				m_game_event = new clan::NetGameEvent("gmsg");
				delete tmp;
			}
		}

		m_canvas.flush();
		m_window.flip();
		
		clan::KeepAlive::process();
	}

	return m_run;
}

bool World::pause()
{
	State::pause();

	m_key_up.disable();
	m_key_down.disable();
	return true;
}

bool World::resume()
{
	State::resume();
	m_key_up.enable();
	m_key_down.enable();
	return true;
}

bool World::exit()
{
	m_client_con->disconnect();
	delete m_gom;
	m_key_up.destroy();
	m_key_down.destroy();
	return true;
}

void World::on_key_up(const clan::InputEvent & e)
{
	if(e.id == clan::keycode_escape)
		m_run = false;

	else if(m_player)
	{
		if(e.id == clan::keycode_a)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_LEFT);
			MessageUtil::add_message(*m_game_event,msg,true);	
		}
		else if(e.id == clan::keycode_d)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_RIGHT);
			MessageUtil::add_message(*m_game_event,msg,true);
		}
		else if(e.id == clan::keycode_w)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_UP);
			MessageUtil::add_message(*m_game_event,msg,true);
		}
		else if(e.id == clan::keycode_s)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_DOWN);
			MessageUtil::add_message(*m_game_event,msg,true);
		}
		else if(e.id == clan::keycode_space)
		{
			msg.keys = msg.keys& (~EUIKT_ATTACK);
			MessageUtil::add_message(*m_game_event,msg,true);
		}

	}else
	{
		if(e.id == clan::keycode_space)
		{
			msg.keys = msg.keys& (~EUIKT_ATTACK);
			MessageUtil::add_message(*m_game_event,msg,true);
		}
	}
}

void World::on_key_down(const clan::InputEvent & e)
{	
	if(m_player)
	{
		if(e.id == clan::keycode_a)
		{
			msg.keys=msg.keys|EUIKT_MOVE_LEFT;
			MessageUtil::add_message(*m_game_event,msg,true);
		}
		else if(e.id == clan::keycode_d)
		{
			msg.keys=msg.keys|EUIKT_MOVE_RIGHT;
			MessageUtil::add_message(*m_game_event,msg,true);
		}
		else if(e.id == clan::keycode_w)
		{
			msg.keys=msg.keys|EUIKT_MOVE_UP;
			MessageUtil::add_message(*m_game_event,msg,true);
		}
		else if(e.id == clan::keycode_s)
		{
			msg.keys=msg.keys|EUIKT_MOVE_DOWN;
			MessageUtil::add_message(*m_game_event,msg,true);
		}
		else if(e.id == clan::keycode_space)
		{
			msg.keys=msg.keys|EUIKT_ATTACK;
			MessageUtil::add_message(*m_game_event,msg,true);
		}
	}
	else
	{
		if(e.id == clan::keycode_space)
		{
			msg.keys=msg.keys|EUIKT_ATTACK;
			MessageUtil::add_message(*m_game_event,msg,true);
		}
	}
}