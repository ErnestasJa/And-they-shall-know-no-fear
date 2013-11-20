#include "precomp.h"

#include "world.h"
#include "tile_chunk.h"
#include "net\user_client.h"
#include "game_objects\go_sprite.h"


World::World(clan::DisplayWindow &display_window)
{
	clan::SetupNetwork setup_network;
	clan::ConsoleLogger logger;

	m_window = display_window;
	m_game_time = clan::GameTime(20,60);

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
	m_gom = new GameObjectManager();
	m_tile_map = TileMap(m_canvas);
	m_tile_map.add_sprite(clan::Sprite::resource(m_canvas,"level_gfx",m_resources),0);
	m_tile_map.load(level);
}

bool World::init()
{
	Message::register_messages();
	PropertyContainer::register_properties();

	m_canvas = clan::Canvas(m_window);

	// Setup resources
	m_resources =   clan::XMLResourceManager::create(clan::XMLResourceDocument("resources.xml"));
	m_run = true;
	
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &World::on_key_up);
	m_key_down = m_window.get_ic().get_keyboard().sig_key_down().connect(this, &World::on_key_down);
	
	m_net_slots.connect(m_client_con->sig_connected(),this, &World::on_connected);
	m_net_slots.connect(m_client_con->sig_disconnected(),this, &World::on_disconnected);
	m_net_slots.connect(m_client_con->sig_event_received(),this, &World::on_net_event);

	clan::log_event("system", "ClientConnection trying to connect");
	m_client_con->connect("62.80.252.115","27015");

	m_client_name = "Skell";
	
	return true;
}

void World::on_connected()
{
	clan::log_event("net_event","Connected to server.");

	msg_auth.name = m_client_name;
	msg_auth.timestamp = m_game_time.get_current_time_ms();
	m_client_con->send_message(msg_auth);
}

void World::on_net_event(const clan::NetGameEvent & e)
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

			MSG_Query q;
			q.query_type = EQT_SERVER_INFO;
			m_client_con->send_message(q);
		}
		else
		{
			clan::log_event("net_event",m.msg);
			m_client_con->disconnect();
		}
	}
	else if(type==MSGS_SERVER_INFO)
	{
		MSG_Server_Info m;
		m.net_deserialize(e.get_argument(1));
		
		m_max_clients = m.max_client_count;
		init_level(m.get_property<std::string>("name"));

		m_clients = new Client[m_max_clients];
		m_players = new GOSprite * [m_max_clients];
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
	if(m_client)
	{
		if(type==MSGC_INPUT)
		{
			MSGC_Input m;
			m.net_deserialize(e.get_argument(1));
			m_players[m.id]->on_message(m);
		}
		else if(type==MSGS_GAME_OBJECT_ACTION)
		{
			MSGS_GameObjectAction m;
			m.net_deserialize(e.get_argument(1));
		
			if(m.action_type==EGOAT_CREATE)
			{
				GameObject * o = m_gom->add_game_object(m.object_type,m.guid);

				o->load_properties(m.object_properties);

				if(o->get_type()==EGOT_SPRITE)
				{
					GOSprite * spr = static_cast<GOSprite*>(o);

					if(o->get_guid()==m_client->get_id())
						m_player = spr;

					m_players[spr->get_guid()]=spr;

					spr->load(m_canvas,m_resources);
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

			m_tile_map.render(m_pos);
			m_gom->update_game_objects(m_game_time);
			m_gom->render_game_objects(m_canvas);
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
			m_player->on_message(msg);
			m_client_con->send_message(msg,false);
		}
		else if(e.id == clan::keycode_d)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_RIGHT);
			m_player->on_message(msg);
			m_client_con->send_message(msg,false);
		}
		else if(e.id == clan::keycode_w)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_UP);
			m_player->on_message(msg);
			m_client_con->send_message(msg,false);
		}
		else if(e.id == clan::keycode_s)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_DOWN);
			m_player->on_message(msg);
			m_client_con->send_message(msg,false);
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
			m_player->on_message(msg);
			m_client_con->send_message(msg,false);
		}
		else if(e.id == clan::keycode_d)
		{
			msg.keys=msg.keys|EUIKT_MOVE_RIGHT;
			m_player->on_message(msg);
			m_client_con->send_message(msg,false);
		}
		else if(e.id == clan::keycode_w)
		{
			msg.keys=msg.keys|EUIKT_MOVE_UP;
			m_player->on_message(msg);
			m_client_con->send_message(msg,false);
		}
		else if(e.id == clan::keycode_s)
		{
			msg.keys=msg.keys|EUIKT_MOVE_DOWN;
			m_player->on_message(msg);
			m_client_con->send_message(msg,false);
		}
	}
}