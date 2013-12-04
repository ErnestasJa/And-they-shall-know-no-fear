#ifndef APP_H
#define APP_H

#include "properties/property_container.h"
#include "properties/property.h"

class State;
class App: public PropertyContainer
{
///settings
public:
	Property<std::string> window_title;
	Property<std::string> console_title;
	Property<std::string> ip;
	Property<std::string> port;
	Property<std::string> username;
	virtual void init_default_settings();

protected:
	bool m_quit;
	std::stack<State*> m_states;

public:
	App() {};

	virtual int main(const std::vector<std::string> &args);
	
	std::stack<State*> & get_states();
	void on_window_close();
};

#endif // APP_H
