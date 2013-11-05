#include "precomp.h"

#include "world.h"
#include "tile_chunk.h"
#include "game_objects\go_sprite.h"

World::World(clan::DisplayWindow &display_window)
{
	m_window = display_window;
	m_game_time = clan::GameTime(20,60);
}

World::~World()
{

}

void World::init_level()
{
	Message::register_messages();
	m_gom = new GameObjectManager();
	m_tile_map = TileMap(m_canvas);
	m_tile_map.add_sprite(clan::Sprite::resource(m_canvas,"level_gfx",m_resources),0);

	m_tile_map.load("test.map");

	spr = static_cast<GOSprite *>(m_gom->add_game_object(EGOT_SPRITE,0));
	spr->load(m_canvas,m_resources);
}

bool World::init()
{
	m_canvas = clan::Canvas(m_window);

	// Setup resources
	m_resources =   clan::XMLResourceManager::create(clan::XMLResourceDocument("resources.xml"));
	m_run = true;
	
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &World::on_key_up);
	m_key_down = m_window.get_ic().get_keyboard().sig_key_down().connect(this, &World::on_key_down);
	///load level
	init_level();

	return true;
}


bool World::run()
{
	if(m_run)
	{
		m_game_time.update();
		m_canvas.clear();

		//m_tile_map.render(m_pos);
		m_gom->update_game_objects(m_game_time);
		m_gom->render_game_objects(m_canvas);

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
	delete m_gom;
	m_key_up.destroy();
	m_key_down.destroy();
	return true;
}

void World::on_key_up(const clan::InputEvent & e)
{
	if(e.id == clan::keycode_escape)
		m_run = false;

	
	else if(e.id == clan::keycode_a)
	{
		msg.keys.data()&= ~EUIKT_MOVE_LEFT;
		spr->on_message(msg);
	}
	else if(e.id == clan::keycode_d)
	{
		msg.keys.data()&= ~EUIKT_MOVE_RIGHT;
		spr->on_message(msg);
	}
	else if(e.id == clan::keycode_w)
	{
		msg.keys.data()&= ~EUIKT_MOVE_UP;
		spr->on_message(msg);
	}
	else if(e.id == clan::keycode_s)
	{
		msg.keys.data()&= ~EUIKT_MOVE_DOWN;
		spr->on_message(msg);
	}
	clan::Console::write_line("Released: %1", e.id);
}

void World::on_key_down(const clan::InputEvent & e)
{
	if(e.id == clan::keycode_a)
	{
		msg.keys.data()|=EUIKT_MOVE_LEFT;
		spr->on_message(msg);
	}
	else if(e.id == clan::keycode_d)
	{
		msg.keys.data()|=EUIKT_MOVE_RIGHT;
		spr->on_message(msg);
	}
	else if(e.id == clan::keycode_w)
	{
		msg.keys.data()|=EUIKT_MOVE_UP;
		spr->on_message(msg);
	}
	else if(e.id == clan::keycode_s)
	{
		msg.keys.data()|=EUIKT_MOVE_DOWN;
		spr->on_message(msg);
	}
	clan::Console::write_line("Pressed: %1", e.id);
}