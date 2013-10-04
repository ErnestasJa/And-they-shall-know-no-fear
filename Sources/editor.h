#pragma once

#include "state.h"
#include "tile_map.h"

class editor: public State
{
public:
	editor(clan::DisplayWindow &display_window);
	~editor();

	bool init();
	bool run();
	bool exit();

	bool pause();
	bool resume();

protected:
	///event funcs
	void on_input(const clan::InputEvent & e);

	///game specific funcs
	void init_level();
	void edge_pan(const clan::vec2 & pos);
	void draw_world_axis();

protected:
	bool m_run;
	clan::Slot m_key_up;
	clan::Slot m_mouse_click;
	clan::Slot m_mouse_move;

	clan::ResourceManager	m_resources;
	clan::DisplayWindow		m_window;
	clan::Canvas			m_canvas;
	clan::GameTime			m_game_time;

	///Kameros pozicija
	clan::vec2 m_pos;

	///Kameros poslinkis
	clan::vec2 m_pan;

	///game
	TileMap m_tile_map;
};
