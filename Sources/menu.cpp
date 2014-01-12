#include "precomp.h"
#include "menu.h"
#include <ClanLib/gl.h>
#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>

Menu::Menu(App * app, clan::DisplayWindow & wnd)
{
	m_app = app;
	m_window = wnd;
	m_canvas = clan::Canvas(m_window);
	m_run = true;
}

bool Menu::init()
{
	m_change=0;

	clan::XMLResourceDocument res("menu.xml");
	m_resources = clan::XMLResourceManager::create(res);
	
	m_background = clan::Image::resource(m_canvas,"background",m_resources);
	button_exit_img = clan::Image::resource(m_canvas,"quit_button_img",m_resources);
	button_world_img = clan::Image::resource(m_canvas,"play_button_img",m_resources);
	button_editor_img = clan::Image::resource(m_canvas,"editor_button_img",m_resources);
	button_sound_on_img = clan::Image::resource(m_canvas,"sound_button_on_img",m_resources);
	button_sound_off_img = clan::Image::resource(m_canvas,"sound_button_off_img",m_resources);
	m_window_manager = clan::GUIWindowManagerDirect(m_window, m_canvas);	
	m_gui_manager = clan::GUIManager(m_window_manager, "Gfx/gui/aero");
	c = new clan::GUIComponent(&m_gui_manager, clan::GUITopLevelDescription(clan::Rect(0,0,1024,720),true),"rootx");
	windowClosedEventSlot = m_window.sig_window_close().connect(this, &Menu::WindowCloseEventHandler);

	button_world = new clan::PushButton(c);	
	button_world->set_geometry(clan::Rect( 422, 490, clan::Size(180, 28)));
	button_world->func_clicked().set(this, &Menu::on_button_clicked, button_world);
	button_world_top = new clan::ImageView(button_world);
	button_world_top->set_geometry(clan::Rect( -92, -18, clan::Size(364, 68)));
	button_world_top->set_image(button_world_img);
	button_world_top->set_scale_to_fit(true);
	
	
	button_editor = new clan::PushButton(c);
	button_editor->set_geometry(clan::Rect( 422, 540, clan::Size(180, 28)));
	button_editor->func_clicked().set(this, &Menu::on_button_clicked, button_editor);
	button_editor_top = new clan::ImageView(button_editor);
	button_editor_top->set_geometry(clan::Rect( -92, -18, clan::Size(364, 68)));
	button_editor_top->set_image(button_editor_img);
	button_editor_top->set_scale_to_fit(true);
	
	
	button_exit = new clan::PushButton(c);
	button_exit->set_geometry(clan::Rect( 422, 590, clan::Size(180, 28)));
	button_exit->func_clicked().set(this, &Menu::on_button_clicked, button_exit);
	button_exit_top = new clan::ImageView(button_exit);
	button_exit_top->set_geometry(clan::Rect( -92, -18, clan::Size(364, 68)));
	button_exit_top->set_image(button_exit_img);
	button_exit_top->set_scale_to_fit(true);
	
	m_con_window = new ConDialogue(c);
	m_con_window->set_default_values(m_app->ip,m_app->port,m_app->username,"pass");

	m_exit_window = new YNDialogue(c, "You are about to quit!");

	confirmWindowClosedEventSlot = m_exit_window->confirmation().connect(this, &Menu::CloseConfirmed);
	connectAttemptEventSlot = m_con_window->submit_connection().connect(this, &Menu::ConnectAttempt);

	///reikalinga eilute norint sutvarkyti kai kuriu elementu matomuma. (gui posistemes bug'as?)
	c->update_layout();

	//MUSIC BY Kevin MacLeod
	clan::SoundBuffer back_sound("Angevin.ogg");
	back_sound.set_volume(0.5f);
	playback = back_sound.prepare();
	//playback.play();
	play = false;
	playback.set_looping(true);

		//sound migtukas
	button_sound = new clan::PushButton(c);
	button_sound->set_geometry(clan::Rect( 0, 0, clan::Size(20, 20)));
	button_sound->func_clicked().set(this, &Menu::on_button_clicked, button_sound);
	button_sound_top = new clan::ImageView(button_sound);
	button_sound_top->set_geometry(clan::Rect(0, 0, clan::Size(20, 20)));
	button_sound_top->set_image(button_sound_on_img);
	button_sound_top->set_scale_to_fit(true);

	return true;
}

void Menu::ConnectAttempt(const ConnectionInfo &info)
{//DEBUG NOT FINISHED! connect params with world
	
	State * s = new World(m_window);
	static_cast<World*>(s)->set_info(info);

	if(s->init())
	{
		m_app->get_states().push(s);
		pause();
	}
	else
	{
		s->exit();
		delete s;
	}
}

bool Menu::run()
{
	if(m_run)
	{
		changing();

		m_background.draw(m_canvas,clan::Rect(0,0,1024,720));

		///render gui
		m_gui_manager.process_messages(0);
		m_gui_manager.render_windows();
		m_window_manager.get_canvas(NULL).flush(); 

		m_window.flip(0);
		clan::KeepAlive::process();
	}

	return m_run;
}

void Menu::WindowCloseEventHandler()
{
	m_run = false;
}

void Menu::CloseConfirmed(bool close)
{
	m_run = !close;
}

bool Menu::pause()
{
	State::pause();

	playback.stop();
	button_sound_top->set_image(button_sound_on_img);
	c->set_visible(false);

	m_key_up.disable();

	confirmWindowClosedEventSlot.disable();

	return true;
}

bool Menu::resume()
{
	State::resume();
	if(play)
	{
	button_sound_top->set_image(button_sound_off_img);
	playback.play();
	}
	else 
	button_sound_top->set_image(button_sound_on_img);
	c->set_visible(true);

	m_key_up.enable();

	confirmWindowClosedEventSlot.enable();

	return true;
}

bool Menu::exit()
{
	m_key_up.destroy();
	return true;
}

void Menu::on_button_clicked(clan::PushButton *button)
{
	if(button==button_editor)
	{
		State * s = new editor(m_window);
		if(s->init())
		{
			m_app->get_states().push(s);
			pause();
		}
		else
		{
			s->exit();
			delete s;
		}
	}
	else if(button==button_world)
	{
		m_con_window->toggle_visibility();
	}
	else if(button==button_exit)
	{
		m_exit_window->toggle_visibility();
	}
	else if(button==button_sound)
	{
		if(playback.is_playing())
			{playback.stop();
		button_sound_top->set_image(button_sound_on_img);
		play = false;}
		else
			{playback.play();
		button_sound_top->set_image(button_sound_off_img);
		play = true;}
	}
}

void Menu::changing()
{
	clan::Point current = m_window.get_ic().get_mouse().get_position();

	if(	current.x >414 && current.x<607 && current.y>490 && current.y<518)
	{
		button_world_top->set_geometry(clan::Rect( -94, -20, clan::Size(368, 72)));
		m_change=1;
	}
	else if (current.x >414 && current.x<607 && current.y>540 && current.y<568)
	{
		button_editor_top->set_geometry(clan::Rect( -94, -20, clan::Size(368, 72)));
		m_change=2;
	}
	else if (current.x >414 && current.x<607 && current.y>590 && current.y<618)
	{
		button_exit_top->set_geometry(clan::Rect( -94, -20, clan::Size(368, 72)));
		m_change=3;
	}
	else if(m_change>0)
	{
		if(m_change==1)
			button_world_top->set_geometry(clan::Rect( -92, -18, clan::Size(364, 68)));
		else if(m_change==2)
			button_editor_top->set_geometry(clan::Rect( -92, -18, clan::Size(364, 68)));
		else if(m_change==3)
			button_exit_top->set_geometry(clan::Rect( -92, -18, clan::Size(364, 68)));

		m_change=0;
	}
}