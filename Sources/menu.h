#pragma once

#include "state.h"
#include "world.h"
#include "editor.h"
#include "app.h"

class Menu: public State
{
protected:
	App * m_app;

	clan::Canvas m_canvas;
	clan::DisplayWindow m_window;
	clan::ResourceManager m_resources;
	clan::Image m_background;
	clan::GUIManager m_gui_manager;
	clan::GUIWindowManagerDirect m_window_manager;

	//gui
	clan::PushButton * button_world, *button_editor;
	clan::Label * l, *l2;
	GUIComponent * c;

	bool m_run;
	clan::Slot m_key_up;
public:

	Menu(App * app, clan::DisplayWindow & wnd);
	
	bool init();
	bool run();
	bool pause();
	bool resume();
	bool exit();

	void on_key_up(const InputEvent & e);
	void on_button_clicked(clan::PushButton *button);
};