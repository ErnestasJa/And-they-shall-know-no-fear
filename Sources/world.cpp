/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    
*/
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
	m_tile_map.add_sprite(Sprite::resource(m_canvas,"level_gfx",m_resources),0);
	TileChunk c = m_tile_map.add_chunk(vec2(0,0));

	c.get_tile(vec2(0,0),0).type=ETT_NORMAL;
	c.get_tile(vec2(0,0),0).sprite_frame=113;

	c.get_tile(vec2(0,0),1).type=ETT_NORMAL;
	c.get_tile(vec2(0,0),1).sprite_frame=117;
}

bool World::init()
{
	m_canvas = clan::Canvas(m_window);

	clan::BlendStateDescription blend_desc;
	blendstate_default = clan::BlendState(m_canvas, blend_desc);
	blend_desc.set_blend_function(clan::blend_zero, clan::blend_one_minus_src_alpha, clan::blend_zero, clan::blend_one_minus_src_alpha);
	blendstate_cl_blend_zero_cl_blend_one_minus_src_alpha = clan::BlendState(m_canvas, blend_desc);
	blend_desc.set_blend_function(clan::blend_src_alpha, clan::blend_one_minus_src_alpha, clan::blend_src_alpha, clan::blend_one_minus_src_alpha);
	blendstate_cl_blend_src_alpha_cl_blend_one_minus_src_alpha = clan::BlendState(m_canvas, blend_desc);

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

void World::on_key_up(const InputEvent & e)
{
	if(e.id == clan::keycode_w)
		m_run = false;
}