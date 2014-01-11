#ifndef MENU_H
#define MENU_H

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
	clan::Image m_background,button_exit_img,button_world_img,button_editor_img, sound_button_img;
	clan::GUIManager m_gui_manager;
	clan::GUIWindowManagerDirect m_window_manager;
	

	//gui
	clan::ImageView *button_world_top,*button_editor_top,*button_exit_top;
	clan::PushButton *button_world, *button_editor, *button_exit, *sound_button;
	clan::SoundBuffer_Session playback;
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
	int pokitis;

	void on_key_up(const clan::InputEvent & e);
	void on_button_clicked(clan::PushButton *button);
	void kitimas();
};

#endif // MENU_H
