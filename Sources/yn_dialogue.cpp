#include "precomp.h"
#include "yn_dialogue.h"



YNDialogue::YNDialogue(clan::GUIComponent * root, std::string message):clan::GUIComponent(root)
{
	m_exit_window = new clan::Window(root);
	m_exit_window->set_geometry(clan::Rect( 300, 460, clan::Size(200,120)));
	m_exit_window->set_title("Are you sure?");
	m_exit_window->func_close().set(this, &YNDialogue::close_window);
	m_exit_window->set_visible(false);

	m_lable = new clan::Label(m_exit_window);
	m_lable->set_geometry(clan::Rect(20, 35, clan::Size(160, 30))); 
	m_lable->set_text(message);

	m_button_exit_Y = new clan::PushButton(m_exit_window);
	m_button_exit_Y->set_geometry(clan::Rect( 20, 70, clan::Size(80, 30)));
	m_button_exit_Y->func_clicked().set(this, &YNDialogue::on_button_clicked, m_button_exit_Y);
	m_button_exit_Y->set_text("YES");

	m_button_exit_N = new clan::PushButton(m_exit_window);
	m_button_exit_N->set_geometry(clan::Rect( 100, 70, clan::Size(80, 30)));
	m_button_exit_N->func_clicked().set(this, &YNDialogue::on_button_clicked, m_button_exit_N);
	m_button_exit_N->set_text("NO");
}

YNDialogue::~YNDialogue()
{
	
}

bool YNDialogue::close_window()
{
    m_exit_window->set_visible(false);
	return  m_exit_window->is_visible();
}

void YNDialogue::on_button_clicked(clan::PushButton *button)
{
	if(button==m_button_exit_Y)
	{
		confirmed=true;
		m_sig.invoke(confirmed);
	}
	else if(button==m_button_exit_N)
	{
		confirmed=false;
		m_sig.invoke(confirmed);

		m_exit_window->set_visible(false);
	}
}

void YNDialogue::toggle_visibility()
{
	m_exit_window->set_visible(!m_exit_window->is_visible());
}

clan::Signal_v1<bool> & YNDialogue::confirmation()
{
	return m_sig;
}

