#include "precomp.h"
#include "menu.h"

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
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &Menu::on_key_up);

	m_window_manager = clan::GUIWindowManagerDirect(m_window, m_canvas);
	m_gui_manager = clan::GUIManager(m_window_manager, "Gfx/gui/aero");
	c = new GUIComponent(&m_gui_manager, GUITopLevelDescription(Rect(0,0,1024,720),true),"rootx");
	c->set_constant_repaint(true);

	button_world = new PushButton(c);
	button_world->set_geometry(clan::Rect( 540, 300, clan::Size(160, 60)));
	button_world->func_clicked().set(this, &Menu::on_button_clicked, button_world);
	button_world->set_text("World");

	button_editor = new PushButton(c);
	button_editor->set_geometry(clan::Rect( 540, 380, clan::Size(160, 60)));
	button_editor->func_clicked().set(this, &Menu::on_button_clicked, button_editor);
	button_editor->set_text("Editor");

	///reikalinga eilute norint sutvarkyti kai kuriu elementu matomuma. (gui posistemes bug'as?)
	c->update_layout();

	return true;
}

bool Menu::run()
{
	if(m_run)
	{
		m_background.draw(m_canvas,Rect(0,0,1024,720));
		m_gui_manager.process_messages(0);
		m_gui_manager.render_windows();

		m_window_manager.get_canvas(NULL).flush(); 
		m_window.flip(0);
		clan::KeepAlive::process();
	}

	return m_run;
}

bool Menu::pause()
{
	State::pause();

	m_key_up.disable();
	return true;
}

bool Menu::resume()
{
	State::resume();

	m_key_up.enable();
	return true;
}

bool Menu::exit()
{
	m_key_up.destroy();
	return true;
}

void Menu::on_key_up(const InputEvent & e)
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
	else if(button==button)
	{
		State * s = new World(m_window);
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
}