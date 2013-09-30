#include "precomp.h"

#include "editor.h"
#include "TileChunk.h"

editor::editor(clan::DisplayWindow &display_window)
{
	m_window = display_window;
}

editor::~editor()
{

}

void editor::init_level()
{
	m_tile_map = TileMap(m_canvas);
	m_tile_map.add_sprite(Sprite::resource(m_canvas,"level_gfx",m_resources),0);
	TileChunk c = m_tile_map.add_chunk(vec2(0,0));

	c.get_tile(vec2(0,0),0).type=ETT_NORMAL;
	c.get_tile(vec2(0,0),0).sprite_frame=113;

	c.get_tile(vec2(0,0),1).type=ETT_NORMAL;
	c.get_tile(vec2(0,0),1).sprite_frame=117;
}

bool editor::init()
{
	m_canvas = clan::Canvas(m_window);

	// Setup resources
	m_resources =   clan::XMLResourceManager::create(clan::XMLResourceDocument("resources.xml"));
	m_run = true;
	
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &editor::on_key_up);

	// load level
	init_level();

	return true;
}


bool editor::run()
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


bool editor::pause()
{
	State::pause();

	m_key_up.disable();
	return true;
}
bool editor::resume()
{
	State::resume();

	m_key_up.enable();
	return true;
}
bool editor::exit()
{
	m_key_up.destroy();
	return true;
}

void editor::on_key_up(const InputEvent & e)
{
	if(e.id == clan::keycode_w)
		m_run = false;
	else if (e.id == clan::keycode_e)
		m_run = false;
}