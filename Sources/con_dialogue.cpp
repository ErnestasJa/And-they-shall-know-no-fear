#include "precomp.h"
#include "con_dialogue.h"



ConDialogue::ConDialogue(clan::GUIComponent * root):clan::GUIComponent(root)
{

	m_connection_window = new clan::Window(root);
	m_connection_window->set_geometry(clan::Rect( 250, 250, clan::Size(200,335) ));
	m_connection_window->set_title("Please provide information");
	m_connection_window->func_close().set(this, &ConDialogue::close_window);
	m_connection_window->set_visible(false);
	m_connection_window->set_focus(false);

	//

	m_lable = new clan::Label(m_connection_window);
	m_lable->set_geometry(clan::Rect(20, 30, clan::Size(160, 30) )); 
	m_lable->set_text("Server IP adress:");
	m_line_IP = new clan::LineEdit(m_connection_window);
	m_line_IP->set_geometry(clan::Rect(20, 60, clan::Size(160, 20) ));
	m_line_IP->set_text("127.0.0.1");

	m_lable = new clan::Label(m_connection_window);
	m_lable->set_geometry(clan::Rect(20, 90, clan::Size(160, 30) )); 
	m_lable->set_text("Server port:");
	m_line_port = new clan::LineEdit(m_connection_window);
	m_line_port->set_geometry(clan::Rect(20, 120, clan::Size(160, 20) ));
	m_line_port->set_text("27015");

	m_lable = new clan::Label(m_connection_window);
	m_lable->set_geometry(clan::Rect(20, 150, clan::Size(160, 30) )); 
	m_lable->set_text("Server password:");
	m_line_pass = new clan::LineEdit(m_connection_window);
	m_line_pass->set_geometry(clan::Rect(20, 180, clan::Size(160, 20) ));
	m_line_pass->set_text("Neperkandamas kodas");

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
	m_button_reset->set_text("Close");
}

void ConDialogue::set_default_values(const std::string & ip, const std::string & port, const std::string & name, const std::string & password)
{
	m_line_IP->set_text(ip);
	m_line_port->set_text(port);
	m_line_name->set_text(name);
	m_line_pass->set_text(password);
}

ConDialogue::~ConDialogue()
{
	
} 

bool ConDialogue::close_window()
{
    m_connection_window->set_visible(false);
	m_connection_window->set_focus(false);
	return  m_connection_window->is_visible();
}

void ConDialogue::toggle_visibility()
{
	m_connection_window->set_visible(!m_connection_window->is_visible());
	m_connection_window->set_focus(!m_connection_window->has_focus());
}

void ConDialogue::on_button_clicked(clan::PushButton *button)
{
	if(button==m_button_connect)
	{
		m_info.ip=m_line_IP->get_text();
		m_info.port=m_line_port->get_text();
		m_info.pass=m_line_pass->get_text();
		m_info.name=m_line_name->get_text();

		m_sig.invoke(m_info);
		toggle_visibility();
	}
	else if(button==m_button_reset)
	{
		close_window();
	}
}

clan::Signal_v1<const ConnectionInfo&> & ConDialogue::submit_connection()
{
	return m_sig;
} 
