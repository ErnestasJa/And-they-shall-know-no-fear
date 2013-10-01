#pragma once

#include "state.h"
#include "TileMap.h"

class World: public State
{

public:
	World(clan::DisplayWindow &display_window);
	~World();

	bool init();
	bool run();
	bool exit();

	bool pause();
	bool resume();

protected:
	///event funcs
	void on_key_up(const clan::InputEvent & e);

	///game specific funcs
	void init_level();

protected:
	bool m_run;
	clan::Slot m_key_up;

	clan::ResourceManager	m_resources;
	clan::DisplayWindow		m_window;
	clan::Canvas			m_canvas;
	clan::GameTime			m_game_time;

	///Kameros pozicija
	clan::vec2 m_pos;

	///game
	TileMap m_tile_map;
};
