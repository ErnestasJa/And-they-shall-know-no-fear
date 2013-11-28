#include "precomp.h"
#include "editor.h"
#include "tile_chunk.h"
#include "sprite_frame_selection.h"


editor::editor(clan::DisplayWindow &display_window)
{
	m_window = display_window;
	m_sprite_frame_selection = nullptr;
	m_selected_frame = -1;
	m_selected_layer = 0;
	m_selected_sprite_sheet = -1;
	m_game_time = clan::GameTime(20,60);
}

editor::~editor()
{

}

void editor::init_gui()
{
	m_window_manager = clan::GUIWindowManagerDirect(m_window, m_canvas);
	m_gui_manager = clan::GUIManager(m_window_manager, "Gfx/gui/aero");
	m_gui_root = new clan::GUIComponent(&m_gui_manager, clan::GUITopLevelDescription(clan::Rect(0,0,1024,720),true),"rootx");
	
	m_editor_window = new clan::Window(m_gui_root);
	m_editor_window->set_geometry(clan::Rect(m_gui_root->get_content_box().get_width()-150,0,clan::Size(150,200)));
	m_editor_window->set_visible(true);
	m_editor_window->func_close().set<editor, clan::GUIComponent*>(this, &editor::on_close_wnd, (clan::GUIComponent*)m_editor_window);
	
	init_gui_sprite_sheet_dropbox(m_editor_window, clan::Rect(10, 30, clan::Size(80, 25)));
	init_gui_layer_dropbox(m_editor_window, clan::Rect(10, 60, clan::Size(80, 25)));

	m_button_sprite_frame = new clan::PushButton(m_editor_window);
	m_button_sprite_frame->set_geometry(clan::Rect( 10, 90, clan::Size(80, 25)));
	m_button_sprite_frame->func_clicked().set(this, &editor::on_button_clicked, m_button_sprite_frame);
	m_button_sprite_frame->set_text("Select frame");

	init_gui_axis_checkbox(m_editor_window, 10, 120, clan::Size(15, 15));

	m_sprite_selection_window = new clan::Window(m_gui_root);
	m_sprite_selection_window->set_geometry(clan::Rect(200,100,clan::Size(500,500)));
	m_sprite_selection_window->set_visible(false);
	m_sprite_selection_window->func_close().set<editor, clan::GUIComponent*>(this, &editor::on_close_wnd, (clan::GUIComponent*)m_sprite_selection_window);

	m_button_select_resource_file=new clan::PushButton(m_editor_window);
	m_button_select_resource_file->set_geometry(clan::Rect( 10, 150, clan::Size(80, 25)));
	m_button_select_resource_file->set_text("Select resource");
	m_button_select_resource_file->func_clicked().set(this, &editor::on_button_clicked, m_button_select_resource_file);

	m_sprite_frame_selection = new SpriteFrameSelection(m_sprite_selection_window, m_game_time);

	m_frame_select = m_sprite_frame_selection->func_frame_selected().connect(this,&editor::on_frame_select);

	m_gui_root->update_layout();
}

void editor::init_gui_layer_dropbox(clan::Window * root, const clan::Rect pos)
{
	uint32_t i;
	m_combo_layer = new clan::ComboBox(root);
	m_combo_layer->set_geometry(pos);
	for(i=0;i<GROUND_LAYER_COUNT;i++) 
		m_combo_menu_layer.insert_item("Ground "+clan::StringHelp::uint_to_text(i+1),i,i);
	m_combo_menu_layer.insert_separator(i);
	for(i++;i<GROUND_LAYER_COUNT+OBJECT_LAYER_COUNT+1;i++)
		m_combo_menu_layer.insert_item("Object "+clan::StringHelp::uint_to_text(i),i-1,i);

	m_combo_layer->set_popup_menu(m_combo_menu_layer);
	m_combo_layer->set_text("Select layer");
	m_combo_layer->func_item_selected().set(this,&editor::on_layer_select);
}

void editor::init_gui_sprite_sheet_dropbox(clan::Window * root, const clan::Rect pos)
{
	m_combo_sprite_sheet = new clan::ComboBox(root);
	m_combo_sprite_sheet->set_geometry(pos);

	

	m_combo_sprite_sheet->set_popup_menu(m_combo_menu_sprite_sheet);
	m_combo_sprite_sheet->set_text("Select sprite sheet");
	m_combo_sprite_sheet->func_item_selected().set(this,&editor::on_sprite_sheet_select);
}

void editor::update_gui_sprite_sheet_dropbox()
{
	m_combo_menu_sprite_sheet.clear();

	clan::XMLResourceDocument d = m_tile_map.get_resource_document();

	std::vector<std::string> names = d.get_resource_names("sprite_sheet_list/"); ///gale reikalingas "/"

	for(auto it = names.begin(); it != names.end(); it++)
	{
		std::string name = (*it);
		clan::DomElement el = d.get_resource("sprite_sheet_list/"+name).get_element();

		if(el.get_tag_name()=="sprite")
		{
			if (el.has_attribute("name"))
			{
				std::string ss_name = el.get_attribute("name");
				m_combo_menu_sprite_sheet.insert_item(ss_name);
			}
		}
	}

	m_combo_sprite_sheet->set_popup_menu(m_combo_menu_sprite_sheet);
}


void editor::init_gui_axis_checkbox(clan::Window * root, int left, int right, clan::Size size)
{
	m_checkbox_t = new clan::CheckBox(root);
	m_checkbox_t->set_geometry(clan::Rect(left, right, size));
	m_checkbox_t->set_checked(true);
	
	m_checkbox_c = new clan::CheckBox(root);
	m_checkbox_c->set_geometry(clan::Rect(left+=20, right, size));
	m_checkbox_c->set_checked(true);
	
	m_checkbox_o = new clan::CheckBox(root);
	m_checkbox_o->set_geometry(clan::Rect(left+=20, right, size));
	m_checkbox_o->set_checked(true);
}

void editor::init_level()
{
	m_tile_map = TileMap(m_canvas);
	//tile_map.load_resource_document("level_resources.xml");
	m_tile_map.load("Level/next_level.map");
}

bool editor::init()
{
	m_run = true;
	m_canvas = clan::Canvas(m_window);
	
	// Setup resources
	m_resources = clan::XMLResourceManager::create(clan::XMLResourceDocument("resources.xml"));
	
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &editor::on_input);
	m_mouse_up = m_window.get_ic().get_mouse().sig_key_up().connect(this, &editor::on_input);
	m_mouse_down = m_window.get_ic().get_mouse().sig_key_down().connect(this, &editor::on_input);
	m_mouse_move = m_window.get_ic().get_mouse().sig_pointer_move().connect(this, &editor::on_input);


	init_level();
	init_gui();
	update_gui_sprite_sheet_dropbox();

	return true;
}

/*void editor::edge_pan(const clan::vec2 & pos)
{
	int32_t sens=15;

	if (pos.x < 30) m_pan.x=-sens;
	else if (pos.x > m_window.get_viewport().get_width()-30) m_pan.x=sens;
	else m_pan.x=0;

	if (pos.y < 30) m_pan.y=-sens;
	else if (pos.y > m_window.get_viewport().get_height()-30) m_pan.y=sens;
	else m_pan.y=0;
}*/

void editor::draw_world_axis(bool t, bool c, bool o)
{
	int32_t w = m_window.get_viewport().get_width(), h = m_window.get_viewport().get_height();

	//chunk and tile separators
	for(int32_t i=0; i<=w; i++)
		if((i+m_pos.x)%CHUNK_EDGE_LENGTH_PIXELS==0 && t) 
			m_canvas.draw_line(clan::LineSegment2f(clan::vec2(i,0),clan::vec2(i,h)),clan::Colorf::yellow);
		else if((i+m_pos.x)%TILE_SIZE==0 && c) 
			m_canvas.draw_line(clan::LineSegment2f(clan::vec2(i,0),clan::vec2(i,h)),clan::Colorf::blue);

	for(int32_t i=0; i<=h; i++)
		if((i+m_pos.y)%CHUNK_EDGE_LENGTH_PIXELS==0 && t) 
			m_canvas.draw_line(clan::LineSegment2f(clan::vec2(0,i),clan::vec2(w,i)),clan::Colorf::yellow);
		else if((i+m_pos.y)%TILE_SIZE==0 && c)
			m_canvas.draw_line(clan::LineSegment2f(clan::vec2(0,i),clan::vec2(w,i)),clan::Colorf::blue);
		
	//origin axis
	if(m_pos.x<=w && m_pos.x+w>=0 && o) m_canvas.draw_line(clan::LineSegment2f(clan::vec2(-m_pos.x,0),clan::vec2(-m_pos.x,h)),clan::Colorf::red);
	if(m_pos.y<=h && m_pos.y+h>=0 && o) m_canvas.draw_line(clan::LineSegment2f(clan::vec2(0,-m_pos.y),clan::vec2(w,-m_pos.y)),clan::Colorf::red);
}

bool editor::run()
{
	if(m_run)
	{
		m_game_time.update();
		m_canvas.clear();


		m_pos=m_pos + m_pan + m_scroll;

		m_tile_map.render(m_pos);
		draw_world_axis(m_checkbox_t->is_checked(),m_checkbox_c->is_checked(),m_checkbox_o->is_checked());

		///render gui
		m_gui_manager.process_messages(0);
		m_gui_manager.render_windows();
		m_window_manager.get_canvas(NULL).flush();

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
	m_tile_map.save("Level/next_level.map");
	m_key_up.destroy();
	return true;
}

void editor::on_frame_select(int32_t frame)
{
	m_selected_frame = frame;
}

void editor::on_layer_select(int32_t layer)
{
	m_selected_layer = layer;
	clan::Console::write_line("Selected layer:%1",layer); //DEBUG
}

void editor::on_sprite_sheet_select(int32_t sprite_sheet)
{
	m_selected_sprite_sheet = sprite_sheet;
	clan::Console::write_line("Selected sprite sheet:%1",sprite_sheet); //DEBUG
}

void editor::change_tile_sprite(const clan::vec2 & pos, bool remove)
{
	clan::vec2 chunk_pos=pixel_to_chunk_pos(pos+m_pos);
	clan::vec2 tile_pos=pixel_to_tile_pos(pos+m_pos);
	clan::Console::write_line("chunk: x:%1 y:%2\ntile: x:%3 y:%4", chunk_pos.x, chunk_pos.y, tile_pos.x, tile_pos.y); //DEBUG

	TileChunk c=m_tile_map.get_chunk(chunk_pos);

	if(c.is_null()) 
		c = m_tile_map.add_chunk(chunk_pos);

	if(remove)
		c.get_tile(tile_pos,m_selected_layer).type=ETT_NO_TILE;

	else if(m_selected_frame!=-1 && m_selected_layer!=-1)
	{
		c.get_tile(tile_pos,m_selected_layer).type=ETT_NORMAL;
		c.get_tile(tile_pos,m_selected_layer).sprite_frame=m_selected_frame;
	}
}

void editor::on_input(const clan::InputEvent & e)
{
	switch(e.device.get_type())
	{
		case clan::InputDevice::keyboard:
		{
			if(e.id == clan::keycode_escape)
				m_run = false;
			else if (e.id == clan::keycode_d)
				clan::Console::write_line("x:%1 y:%2", m_pos.x,m_pos.y); //DEBUG
			else if (e.id == clan::keycode_e)
				m_editor_window->set_visible(!m_editor_window->is_visible());
			break;
		}
		case clan::InputDevice::pointer:
		{

			if(m_gui_root->get_component_at(e.mouse_pos)!=m_gui_root) break;

			if (e.id == clan::mouse_left)
				change_tile_sprite(e.mouse_pos);
			else if (e.id == clan::mouse_right)
				change_tile_sprite(e.mouse_pos,true);
			else if (e.id == clan::mouse_middle && e.type == clan::InputEvent::pressed)
			{
				m_drag_offset = e.mouse_pos;
				clan::Console::write_line("PRESSED"); //DEBUG
			}
			else if (e.id == clan::mouse_middle && e.type == clan::InputEvent::released)
			{
				m_scroll=clan::vec2();
				clan::Console::write_line("RELEASED"); //DEBUG
			}
			else if (e.type == clan::InputEvent::pointer_moved)
			{
				//edge_pan(e.mouse_pos); DEBUG
				if (e.device.get_keycode(clan::mouse_left))
					change_tile_sprite(e.mouse_pos);
				else if (e.device.get_keycode(clan::mouse_right))
					change_tile_sprite(e.mouse_pos,true);
				else if (e.device.get_keycode(clan::mouse_middle))
					m_scroll=(e.mouse_pos-m_drag_offset)/m_game_time.get_time_elapsed_ms();
			}
			break;
		}
	}
}

void editor::on_button_clicked(clan::PushButton * btn)
{
	if(btn==m_button_sprite_frame)
	{
		m_sprite_frame_selection->set_sprite(m_tile_map.get_sprite(0));
		m_sprite_selection_window->set_visible(!m_sprite_selection_window->is_visible());
	}
	if(btn==m_button_select_resource_file)	
	{
		m_tile_map.load_resource_document(open_file());
		update_gui_sprite_sheet_dropbox();
	}
}

bool editor::on_close_wnd(clan::GUIComponent* wnd)
{
	wnd->set_visible(false);
	return true;
}

std::string editor::open_file()
{
	clan::OpenFileDialog *dialog = new clan::OpenFileDialog(m_gui_root);
	dialog->set_initial_directory(clan::Directory::get_current());
	dialog->show();
	return clan::PathHelp::make_relative(clan::Directory::get_current(), dialog->get_filename(), clan::PathHelp::path_type_file);
}