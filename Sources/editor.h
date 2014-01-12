#ifndef EDITOR_H
#define EDITOR_H

#include "state.h"
#include "tile_map.h"
#include "yn_dialogue.h"
#include "selected_sprites.h"

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
	void on_frame_select(clan::Rect frame);
	void on_layer_select(int32_t layer);
	void on_sprite_sheet_select(int32_t sprite_sheet);
	void on_spawn_select(int32_t spawn);
	bool on_close_wnd(clan::GUIComponent * wnd);
	void on_exit(bool confirm);

	///editor specific funcs
	void init_level();
	void init_gui();
	void init_gui_layer_dropbox(clan::Window * root, const clan::Rect pos);
	void init_gui_sprite_sheet_dropbox(clan::Window * root, const clan::Rect pos);
	void init_gui_spawn_dropbox(clan::Window * root, const clan::Rect pos);
	void init_gui_load_map_button(clan::Window * root, const clan::Rect pos);
	void init_gui_save_map_button(clan::Window * root, const clan::Rect pos);
	void update_gui_sprite_sheet_dropbox();
	void init_gui_axis_checkbox(clan::Window * root, int left, int right, clan::Size size);
	void edge_pan(const clan::vec2 & pos);
	void draw_world_axis(bool t, bool c, bool w);
	void draw_hover_box(bool draw);
	void draw_selection_box(const clan::Point & pos, bool draw);
	void change_tile_sprite(const clan::vec2 & pos, bool remove=false);
	bool editor::open_file(std::string & str);
	bool editor::save_file(std::string & str);

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

	///Kameros poslinki
	clan::vec2 m_pos,
		m_pan,
		m_offset,
		m_scroll;

	///game
	TileMap m_tile_map;

	///gui
	clan::GUIManager				m_gui_manager;
	clan::GUIWindowManagerDirect	m_window_manager;
	YNDialogue						*m_save_window;

	///gui elements
	clan::GUIComponent			*m_gui_root;
	clan::PushButton			*m_button_multi_tile, 
								*m_button_spawn,
								*m_button_sprite_frame, 
								*m_button_select_resource_file, 
								*m_button_load_map, 
								*m_button_save_map;
	clan::ComboBox				*m_combo_layer, 
								*m_combo_sprite_sheet,
								*m_combo_spawn;
	clan::PopupMenu				m_combo_menu_layer, 
								m_combo_menu_sprite_sheet,
								m_combo_menu_spawn;
	clan::CheckBox				*m_checkbox_t, 
								*m_checkbox_c, 
								*m_checkbox_o;
	clan::Window				*m_sprite_selection_window, 
								*m_editor_window;
	SpriteFrameSelection		*m_sprite_frame_selection;


	clan::Slot m_frame_select, m_save;
	int32_t m_selected_layer, m_selected_sprite_sheet, m_selected_spawn_type;
	SelectedSprites *m_selected_sprites;

};

#endif // EDITOR_H
