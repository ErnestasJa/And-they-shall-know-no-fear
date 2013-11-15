#pragma once

class YNDialogue: public clan::GUIComponent
{

protected:
	bool confirmed;

	clan::Window *m_exit_window;
	clan::Label *m_lable;
	clan::PushButton *m_button_exit_Y, *m_button_exit_N;

	clan::Signal_v1<bool> m_sig;

public:
	YNDialogue(clan::GUIComponent * root, std::string message);
	virtual ~YNDialogue();
	clan::Signal_v1<bool> & YNDialogue::confirmation();
	void render(clan::Canvas & c);
	void on_button_clicked(clan::PushButton *button);
	void toggle_visibility();
	bool close_window();
};