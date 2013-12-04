#include "precomp.h"

#include "app.h"
#include "world.h"
#include "menu.h"

int App::main(const std::vector<std::string> &args)
{
	m_quit = false;

	clan::SetupCore setup_core;
	clan::SetupDisplay setup_display;
	clan::SetupGL setup_gl;
	clan::SetupSWRender setup_swrender;
	clan::SetupGUI setup_gui;
	clan::SetupSound setup_sound;
	clan::SoundOutput output(44100);

	init_default_settings();

	clan::DisplayWindowDescription desc;
	desc.set_title(m_window_title);
	desc.set_size(clan::Size(1024, 720), true);
	clan::DisplayWindow window(desc);

	clan::SetupNetwork setup_network;
	clan::ConsoleWindow console(m_console_title, 160, 1000);
	clan::ConsoleLogger logger;

	clan::Slot slot_quit = window.sig_window_close().connect(this, &App::on_window_close);

	State * start = new Menu(this,window);
	if(!start->init())
	{
		delete start;
	}
	else
		m_states.push(start);


	///run

	while(m_states.size()>0)
	{
		if(m_states.top()->is_paused())
			m_states.top()->resume();

		if(!m_states.top()->run())
		{
			State * s = m_states.top();
			m_states.pop();
			s->exit();
			delete s;
		}
	}

	return 0;
}

void App::init_default_settings()
{
	m_window_title = add_property<std::string>("window_title","And they shall know no fear");
	m_console_title = add_property<std::string>("console_title","Client console");

	if(!clan::FileHelp::file_exists("Cfg/config.xml"))
	{
		try
		{
			clan::File f("Cfg/config.xml",clan::File::create_always,clan::File::access_write);
			clan::DomDocument d;
			clan::DomComment c(d,"This document is for storing application settings.");
			d.append_child(c);
			xml_serialize(d,d.get_first_child().to_element(),true);
			d.save(f);
			f.close();
		}
		catch(clan::Exception &)
		{
			
		}
	}
	else
	{
		try
		{
			clan::File f("Cfg/config.xml",clan::File::open_existing,clan::File::access_read);
			clan::DomDocument d;
			d.load(f);
			xml_deserialize(d.get_elements_by_tag_name("property_containter").item(0).to_element());
			f.close();
		}
		catch(clan::Exception &)
		{
			
		}
	}
}

void App::on_window_close()
{
	m_quit = true;
}

std::stack<State*> & App::get_states()
{
	return m_states;
}

int main_func(const std::vector<std::string> & args)
{
	App app;
	return app.main(args);
}

clan::Application app(&main_func);
