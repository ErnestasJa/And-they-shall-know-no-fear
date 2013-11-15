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
	clan::Window *m_exit_window;
	clan::PushButton * button_world, *button_editor, *button_exit, *button_exit_Y, *button_exit_N;
	clan::GUIComponent * c;

	void WindowCloseEventHandler();
	bool Close_exit_YN_window();
	bool m_run;

	clan::Slot m_key_up;
	clan::Slot windowClosedEventSlot;
public:

	Menu(App * app, clan::DisplayWindow & wnd);
	
	bool init();
	bool run();
	bool pause();
	bool resume();
	bool exit();
	

	void on_key_up(const clan::InputEvent & e);
	void on_button_clicked(clan::PushButton *button);
};