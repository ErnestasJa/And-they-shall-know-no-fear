#include "precomp.h"
#include "utility.h"
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
	m_tile_map.add_sprite(clan::Sprite::resource(m_canvas,"level_gfx",m_resources),0);
	TileChunk c = m_tile_map.add_chunk(clan::vec2(0,0));

	c.get_tile(clan::vec2(0,0),0).type=ETT_NORMAL;
	c.get_tile(clan::vec2(0,0),0).sprite_frame=113;

	c.get_tile(clan::vec2(15,15),1).type=ETT_NORMAL;
	c.get_tile(clan::vec2(15,15),1).sprite_frame=117;
}

bool editor::init()
{
	m_canvas = clan::Canvas(m_window);

	// Setup resources
	m_resources =   clan::XMLResourceManager::create(clan::XMLResourceDocument("resources.xml"));
	m_run = true;
	
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &editor::on_input);
	m_mouse_click = m_window.get_ic().get_mouse().sig_key_up().connect(this, &editor::on_input);

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


void editor::on_input(const clan::InputEvent & e)
{
	switch(e.device.get_type())
	{
		case clan::InputDevice::keyboard:
		{
			if(e.id == clan::keycode_w)
				m_run = false;
			else if (e.id == clan::keycode_e)
				m_run = false;
			break;
		}
		case clan::InputDevice::pointer:
		{
			if (e.id == clan::mouse_left)
			{
				clan::vec2 click_pos=e.mouse_pos;
				m_pos.x=-100;m_pos.y=-100;//DEBUG
				clan::vec2 chunk_pos=pixel_to_chunk_pos(click_pos+m_pos);
				clan::vec2 tile_pos=pixel_to_tile_pos(click_pos+m_pos);
				clan::Console::write_line("chunk: x:%1 y:%2\ntile: x:%3 y:%4",chunk_pos.x, chunk_pos.y, tile_pos.x, tile_pos.y); //DEBUG
			}
			break;
		}
	}
}