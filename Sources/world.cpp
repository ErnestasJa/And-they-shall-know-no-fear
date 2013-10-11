#include "precomp.h"

#include "world.h"
#include "tile_chunk.h"
#include "game_objects\go_sprite.h"

World::World(clan::DisplayWindow &display_window)
{
	m_window = display_window;
	m_gom = new GameObjectManager();
}

World::~World()
{
	delete m_gom;
}

void World::init_level()
{
	m_tile_map = TileMap(m_canvas);
	m_tile_map.add_sprite(clan::Sprite::resource(m_canvas,"level_gfx",m_resources),0);

	m_tile_map.load("test.map");

	m_gom->add_game_object(new GOSprite());
}

bool World::init()
{
	m_canvas = clan::Canvas(m_window);

	// Setup resources
	m_resources =   clan::XMLResourceManager::create(clan::XMLResourceDocument("resources.xml"));
	m_run = true;
	
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &World::on_key_up);

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

		m_tile_map.render(m_pos);

		m_window.flip(1);
		clan::KeepAlive::process();
	}

	return m_run;
}


bool World::pause()
{
	State::pause();

	m_key_up.disable();
	return true;
}

bool World::resume()
{
	State::resume();

	m_key_up.enable();
	return true;
}

bool World::exit()
{
	m_key_up.destroy();
	return true;
}

void World::on_key_up(const clan::InputEvent & e)
{
	if(e.id == clan::keycode_w)
		m_run = false;
}