#pragma once

#include "state.h"
#include "tile_map.h"


class SpriteSelection;
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
	void on_button_clicked(clan::PushButton * btn);
	void on_frame_select(int32_t frame);

	///game specific funcs
	void init_level();
	void init_gui();
	void edge_pan(const clan::vec2 & pos);
	void draw_world_axis();
	void change_tile_sprite(const clan::vec2 & pos);

protected:
	bool m_run;

	///input event slots
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

	///gui
	clan::GUIManager				m_gui_manager;
	clan::GUIWindowManagerDirect	m_window_manager;
	
	///gui elements
	clan::GUIComponent			*m_gui_root;
	clan::PushButton			*m_button_sprite, *m_button_layer;
	clan::Ribbon				*m_ribbon;
	clan::Window				*m_sprite_selection_window;
	SpriteSelection				*m_sprite_selection;


	clan::Slot m_frame_select;
	int32_t m_selected_frame;
};
