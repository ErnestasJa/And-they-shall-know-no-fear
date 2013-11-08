#pragma once

#include "server.h"

class ServerApp
{
protected:
	bool m_quit;

	Server m_server;
public:
	ServerApp();
	void on_window_close();
	int main(const std::vector<std::string> &args);
};

