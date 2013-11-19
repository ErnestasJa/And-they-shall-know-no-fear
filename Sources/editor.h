#pragma once

#include "state.h"
#include "tile_map.h"


class SpriteFrameSelection;
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
	void on_layer_select(int32_t layer);
	bool on_close_wnd(clan::GUIComponent * wnd);

	///game specific funcs
	void init_level();
	void init_gui();
	void init_gui_layer_dropbox(clan::Window * root, const clan::Rect pos);
	void init_gui_axis_checkbox(clan::Window * root, int left, int right, clan::Size size);
	void edge_pan(const clan::vec2 & pos);
	void draw_world_axis(bool t, bool c, bool w);
	void change_tile_sprite(const clan::vec2 & pos, bool remove=false);
	std::string open_file();

protected:
	bool m_run;

	///input event slots
	clan::Slot m_key_up;
	clan::Slot m_mouse_up;
	clan::Slot m_mouse_down;
	clan::Slot m_mouse_move;

	clan::ResourceManager	m_resources;
	clan::DisplayWindow		m_window;
	clan::Canvas			m_canvas;
	clan::GameTime			m_game_time;

	clan::vec2 m_pos, 
		m_pan, 
		m_drag_offset, 
		m_scroll;

	///game
	TileMap m_tile_map;

	///gui
	clan::GUIManager				m_gui_manager;
	clan::GUIWindowManagerDirect	m_window_manager;
	
	///gui elements
	clan::GUIComponent			*m_gui_root;
	clan::PushButton			*m_button_sprite_frame, *m_button_select_resource_file;
	clan::ComboBox				*m_combo_layer;
	clan::PopupMenu				m_combo_menu_layer;
	clan::CheckBox				*m_checkbox_t, *m_checkbox_c, *m_checkbox_o;
	clan::Window				*m_sprite_selection_window, *m_editor_window;
	SpriteFrameSelection		*m_sprite_frame_selection;

	///Kameros poslinki
	clan::Slot m_frame_select;
	int32_t m_selected_frame, m_selected_layer;
};
