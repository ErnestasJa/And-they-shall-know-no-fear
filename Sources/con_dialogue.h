#ifndef CON_DIALOGUE_H
#define CON_DIALOGUE_H

#include "con_info.h"

class ConDialogue: public clan::GUIComponent
{

protected:
	clan::Window *m_connection_window;
	clan::Label *m_lable;
	clan::LineEdit *m_line_IP, *m_line_port, *m_line_name, *m_line_pass;
	clan::PushButton *m_button_connect, *m_button_reset;

	ConnectionInfo m_info;
	clan::Signal_v1<const ConnectionInfo&> m_sig;

public:
	ConDialogue(clan::GUIComponent * root);
	virtual ~ConDialogue();
	clan::Signal_v1<const ConnectionInfo&> & submit_connection();
	void render(clan::Canvas &);
	void on_button_clicked(clan::PushButton *);
	void toggle_visibility();
	bool close_window();

};

#endif // CON_DIALOGUE_H
