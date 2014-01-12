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
	clan::Image m_background,button_exit_img,button_world_img,button_editor_img,button_sound_on_img,button_sound_off_img;
	clan::GUIManager m_gui_manager;
	clan::GUIWindowManagerDirect m_window_manager;
	

	//gui
	clan::ImageView *button_world_top,*button_editor_top,*button_exit_top,*button_sound_top;
	clan::PushButton *button_world, *button_editor, *button_exit, *button_sound;
	YNDialogue *m_exit_window;
	ConDialogue *m_con_window;
	clan::GUIComponent *c;

	///sound
	clan::SoundBuffer_Session playback;
	clan::SoundBuffer back_sound;

	void WindowCloseEventHandler();
	void CloseConfirmed(bool);
	void ConnectAttempt(const ConnectionInfo&);
	void changing(); //buves "kitimas"

	bool m_run;
	bool play;
	int m_change; //buves "pokitis"

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

#endif // MENU_H
