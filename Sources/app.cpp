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
	
	clan::DisplayWindowDescription desc;
	desc.set_title("Demo");
	desc.set_size(clan::Size(1024, 720), true);
	clan::DisplayWindow window(desc);

	clan::SetupNetwork setup_network;
	clan::ConsoleWindow console("Client console", 160, 1000);
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