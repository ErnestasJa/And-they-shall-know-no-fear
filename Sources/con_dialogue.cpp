#include "precomp.h"
#include "con_dialogue.h"



ConDialogue::ConDialogue(clan::GUIComponent * root):clan::GUIComponent(root)
{

	m_connection_window = new clan::Window(root);
	m_connection_window->set_geometry(clan::Rect( 250, 250, clan::Size(200,335) ));
	m_connection_window->set_title("Please provide information");
	m_connection_window->func_close().set(this, &ConDialogue::close_window);
	m_connection_window->set_visible(true); //DEBUG

	//

	m_lable = new clan::Label(m_connection_window);
	m_lable->set_geometry(clan::Rect(20, 30, clan::Size(160, 30) )); 
	m_lable->set_text("Server IP adress:");
	m_line_IP = new clan::LineEdit(m_connection_window);
	m_line_IP->set_geometry(clan::Rect(20, 60, clan::Size(160, 20) ));
	m_line_IP->set_text("127.0.0.1");

	m_lable = new clan::Label(m_connection_window);
	m_lable->set_geometry(clan::Rect(20, 90, clan::Size(160, 30) )); 
	m_lable->set_text("Servers port:");
	m_line_port = new clan::LineEdit(m_connection_window);
	m_line_port->set_geometry(clan::Rect(20, 120, clan::Size(160, 20) ));
	m_line_port->set_text("27015");

	m_lable = new clan::Label(m_connection_window);
	m_lable->set_geometry(clan::Rect(20, 150, clan::Size(160, 30) )); 
	m_lable->set_text("Servers password:");
	m_line_password = new clan::LineEdit(m_connection_window);
	m_line_password->set_geometry(clan::Rect(20, 180, clan::Size(160, 20) ));
	m_line_password->set_text("Neperkandamas kodas");

	m_lable = new clan::Label(m_connection_window);
	m_lable->set_geometry(clan::Rect(20, 220, clan::Size(160, 30) )); 
	m_lable->set_text("Your name:");
	m_line_name = new clan::LineEdit(m_connection_window);
	m_line_name->set_geometry(clan::Rect(20, 250, clan::Size(160, 20) ));
	m_line_name->set_text("Nepakartojamas vardas");

	//

	m_button_connect = new clan::PushButton(m_connection_window);
	m_button_connect->set_geometry(clan::Rect( 20, 285, clan::Size(75, 30) ));
	m_button_connect->func_clicked().set(this, &ConDialogue::on_button_clicked, m_button_connect);
	m_button_connect->set_text("Connect");

	m_button_reset = new clan::PushButton(m_connection_window);
	m_button_reset->set_geometry(clan::Rect( 105, 285, clan::Size(75, 30) ));
	m_button_reset->func_clicked().set(this, &ConDialogue::on_button_clicked, m_button_reset);
	m_button_reset->set_text("Reset");

}

ConDialogue::~ConDialogue()
{
	
} 

bool ConDialogue::close_window()
{
    m_connection_window->set_visible(false);
	return  m_connection_window->is_visible();
}

void ConDialogue::on_button_clicked(clan::PushButton *button)
{
	if(button==m_button_connect)
	{
		m_sig.invoke(
			m_line_IP->get_text(),
			m_line_port->get_text(),
			m_line_password->get_text(),
			m_line_name->get_text()
			);
	}
	else if(button==m_button_reset)
	{
		m_connection_window->set_visible(false);
	}
}

void ConDialogue::toggle_visibility()
{
	m_connection_window->set_visible(!m_connection_window->is_visible());
}

clan::Signal_v4<std::string, std::string, std::string, std::string> & ConDialogue::submit_connection()
{
	return m_sig;
} 
