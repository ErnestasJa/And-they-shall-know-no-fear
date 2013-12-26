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
	clan::XMLResourceDocument res("menu.xml");
	m_resources = clan::XMLResourceManager::create(res);

	m_background = clan::Image::resource(m_canvas,"background",m_resources);
	quit_button_img = clan::Image::resource(m_canvas,"quit_button_img",m_resources);
	play_button_img = clan::Image::resource(m_canvas,"play_button_img",m_resources);
	
	editor_button_img = clan::Image::resource(m_canvas,"editor_button_img",m_resources);

	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &Menu::on_key_up);

	m_window_manager = clan::GUIWindowManagerDirect(m_window, m_canvas);	
	m_gui_manager = clan::GUIManager(m_window_manager, "Gfx/gui/aero");
	c = new clan::GUIComponent(&m_gui_manager, clan::GUITopLevelDescription(clan::Rect(0,0,1024,720),true),"rootx");
	windowClosedEventSlot = m_window.sig_window_close().connect(this, &Menu::WindowCloseEventHandler);
	
	
	
	button_world = new clan::PushButton(c);	
	button_world->set_geometry(clan::Rect( 590, 316, clan::Size(80, 28)));
	button_world->func_clicked().set(this, &Menu::on_button_clicked, button_world);
	//button_world->set_text("World");	
	button_world_top =new clan::ImageView(button_world);
	button_world_top->set_geometry(clan::Rect( -40, -15, clan::Size(160, 60)));
	button_world_top->set_image(play_button_img);
	button_world_top->set_scale_to_fit(true);


	button_editor = new clan::PushButton(c);
	button_editor->set_geometry(clan::Rect( 590, 396, clan::Size(80, 28)));
	button_editor->func_clicked().set(this, &Menu::on_button_clicked, button_editor);
	//button_editor->set_text("Editor");
	editor_button_top =new clan::ImageView(button_editor);
	editor_button_top->set_geometry(clan::Rect( -40, -15, clan::Size(160, 60)));
	editor_button_top->set_image(editor_button_img);
	editor_button_top->set_scale_to_fit(true);

	
	button_exit = new clan::PushButton(c);
	button_exit->set_geometry(clan::Rect( 590, 476, clan::Size(80, 28)));
	button_exit->func_clicked().set(this, &Menu::on_button_clicked, button_exit);
	//button_exit->set_text("Exit");
	quit_button_top =new clan::ImageView(button_exit);
	quit_button_top->set_geometry(clan::Rect( -40, -15, clan::Size(160, 60)));
	quit_button_top->set_image(quit_button_img);
	quit_button_top->set_scale_to_fit(true);
	

	m_exit_window = new YNDialogue(c, "You are about to quit!");
	m_con_window = new ConDialogue(c);

	confirmWindowClosedEventSlot = m_exit_window->confirmation().connect(this, &Menu::CloseConfirmed);
	connectAttemptEventSlot = m_con_window->submit_connection().connect(this, &Menu::ConnectAttempt);

	///reikalinga eilute norint sutvarkyti kai kuriu elementu matomuma. (gui posistemes bug'as?)
	c->update_layout();

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

	c->set_visible(false);

	m_key_up.disable();
	return true;
}

bool Menu::resume()
{
	State::resume();

	c->set_visible(true);

	m_key_up.enable();
	return true;
}

bool Menu::exit()
{
	m_key_up.destroy();
	return true;
}

void Menu::on_key_up(const clan::InputEvent & e)
{
	if(e.id == clan::keycode_q)
		m_run = false;
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

}
