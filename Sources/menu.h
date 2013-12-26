#pragma once

#include "state.h"
#include "world.h"
#include "editor.h"
#include "app.h"
#include "yn_dialogue.h"
#include "con_dialogue.h"
#include "con_info.h"

class Menu: public State
{
protected:
	App * m_app;

	clan::Canvas m_canvas;
	clan::DisplayWindow m_window;
	clan::ResourceManager m_resources;
	clan::Image m_background,quit_button_img,play_button_img,editor_button_img;
	clan::GUIManager m_gui_manager;
	clan::GUIWindowManagerDirect m_window_manager;
		

	//gui
	clan::ImageView *button_world_top,*editor_button_top,*quit_button_top;
	clan::PushButton *button_world, *button_editor, *button_exit;
	YNDialogue *m_exit_window;
	ConDialogue *m_con_window;
	clan::GUIComponent *c;

	void WindowCloseEventHandler();
	void CloseConfirmed(bool);
	void ConnectAttempt(const ConnectionInfo&);

	bool m_run;

	clan::Slot m_key_up;
	clan::Slot windowClosedEventSlot, confirmWindowClosedEventSlot;
	clan::Slot connectAttemptEventSlot;
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