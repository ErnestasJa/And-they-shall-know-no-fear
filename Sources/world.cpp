#include "precomp.h"

#include "world.h"
#include "TileChunk.h"

World::World(clan::DisplayWindow &display_window)
{
	m_window = display_window;
}

World::~World()
{

}

void World::init_level()
{
	m_tile_map = TileMap(m_canvas);
	m_tile_map.add_sprite(clan::Sprite::resource(m_canvas,"level_gfx",m_resources),0);
	
	/*
	TileChunk c = m_tile_map.add_chunk(clan::vec2(0,0));

	for(int32_t i = 0; i < 16; i++)
	for(int32_t j = 0; j < 16; j++)
	{
		c.get_tile(clan::vec2(j,i),0).type=ETT_NORMAL;
		c.get_tile(clan::vec2(j,i),0).sprite_frame=112; //mid
	}

	for(int32_t i = 0; i < 16; i++)
	{
		c.get_tile(clan::vec2(i,0),0).sprite_frame=80; ///top
		c.get_tile(clan::vec2(i,15),0).sprite_frame=144; // bot

		c.get_tile(clan::vec2(0,i),0).sprite_frame=111; /// left
		c.get_tile(clan::vec2(15,i),0).sprite_frame=113; ///right
	}

	c.get_tile(clan::vec2(0,0),0).sprite_frame=79; /// top left
	c.get_tile(clan::vec2(15,0),0).sprite_frame=81; /// top right
	c.get_tile(clan::vec2(0,15),0).sprite_frame=143; /// bot left
	c.get_tile(clan::vec2(15,15),0).sprite_frame=145; /// bot right
	*/

	m_tile_map.load("test.map");
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