#pragma once
class State;
class App
{
protected:
	bool m_quit;
	std::stack<State*> m_states;
public:
	App() {};

	virtual int main(const std::vector<std::string> &args);

	std::stack<State*> & get_states();
	void on_window_close();
};

