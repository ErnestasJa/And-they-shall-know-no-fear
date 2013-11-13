#include "precomp.h"

#include "world.h"
#include "tile_chunk.h"
#include "net\client.h"
#include "game_objects\go_sprite.h"


World::World(clan::DisplayWindow &display_window)
{
	clan::SetupNetwork setup_network;
	clan::ConsoleLogger logger;

	m_window = display_window;
	m_game_time = clan::GameTime(20,60);
	m_client = new Client();
	m_gom = nullptr;
	m_player = nullptr;
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
	register_property_types();

	m_canvas = clan::Canvas(m_window);

	// Setup resources
	m_resources =   clan::XMLResourceManager::create(clan::XMLResourceDocument("resources.xml"));
	m_run = true;
	
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &World::on_key_up);
	m_key_down = m_window.get_ic().get_keyboard().sig_key_down().connect(this, &World::on_key_down);
	
	
	m_client->set_name("Skell");
	
	m_net_slots.connect(m_client->sig_connected(),this, &World::on_connected);
	m_net_slots.connect(m_client->sig_disconnected(),this, &World::on_disconnected);
	m_net_slots.connect(m_client->sig_event_received(),this, &World::on_net_event);

	clan::log_event("system", "Client trying to connect");
	m_client->connect("192.168.1.100","27015");
	
	return true;
}


void World::on_connected()
{
	clan::log_event("net_event","Connected to server.");
	msg_auth.name = m_client->get_name();
	msg_auth.timestamp = m_game_time.get_current_time_ms();
	m_client->send_message(msg_auth);
}

void World::on_net_event(const clan::NetGameEvent & e)
{
	uint32_t type = e.get_argument(0).to_uinteger();
	clan::log_event("net_event","Got message from server type='%1' msg='%2'.",type,e.to_string());

	if(type==MSGS_AUTH_STATUS)
	{
		MSGS_AuthStatus m;
		m.net_deserialize(e);

		if(m.auth_sucessful)
		{
			m_client->set_id(m.id);
			clan::log_event("net_event",m.msg);

			MSG_Query q;
			q.query_type = EQT_MAP_INFO;
			m_client->send_message(q);
		}
		else
		{
			clan::log_event("net_event",m.msg);
			m_client->disconnect();
		}
	}
	else if(type==MSG_QUERY_RESPONSE)
	{
		MSG_QueryResponse m;
		m.net_deserialize(e);
		if(m.query_type==EQT_MAP_INFO)
		{
			if(m.has_property("name",EPT_STRING))
				init_level(m.get_property<std::string>("name"));
			else
				throw clan::Exception("Server didn't set property 'name'.");
		}
	}
	else if(type==MSGS_GAME_OBJECT_ACTION)
	{
		MSGS_GameObjectAction m;
		m.net_deserialize(e);
		
		if(m.action_type==EGOAT_CREATE)
		{
			GameObject * o = m_gom->add_game_object(m.object_type,m.guid);

			if(o->get_type()==EGOT_SPRITE)
			{
				GOSprite * spr = static_cast<GOSprite*>(o);

				if(o->get_guid()==m_client->get_id())
					m_player = spr;

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
	m_client->disconnect();
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
			m_client->send_message(msg);
		}
		else if(e.id == clan::keycode_d)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_RIGHT);
			m_player->on_message(msg);
			m_client->send_message(msg);
		}
		else if(e.id == clan::keycode_w)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_UP);
			m_player->on_message(msg);
			m_client->send_message(msg);
		}
		else if(e.id == clan::keycode_s)
		{
			msg.keys = msg.keys& (~EUIKT_MOVE_DOWN);
			m_player->on_message(msg);
			m_client->send_message(msg);
		}
	}
	clan::Console::write_line("Released: %1", e.id);
}

void World::on_key_down(const clan::InputEvent & e)
{	
	if(m_player)
	{
		if(e.id == clan::keycode_a)
		{
			msg.keys=msg.keys|EUIKT_MOVE_LEFT;
			m_player->on_message(msg);
			m_client->send_message(msg);
		}
		else if(e.id == clan::keycode_d)
		{
			msg.keys=msg.keys|EUIKT_MOVE_RIGHT;
			m_player->on_message(msg);
			m_client->send_message(msg);
		}
		else if(e.id == clan::keycode_w)
		{
			msg.keys=msg.keys|EUIKT_MOVE_UP;
			m_player->on_message(msg);
			m_client->send_message(msg);
		}
		else if(e.id == clan::keycode_s)
		{
			msg.keys=msg.keys|EUIKT_MOVE_DOWN;
			m_player->on_message(msg);
			m_client->send_message(msg);
		}
	}

	clan::Console::write_line("Pressed: %1", e.id);
}