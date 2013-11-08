#include "precomp.h"
#include "server_app.h"

ServerApp::ServerApp()
{
	
}

int ServerApp::main(const std::vector<std::string> &args)
{
	
	m_server.init(4,"27015");
	
	while(m_server.run())
	{
		clan::KeepAlive::process(10);
	}

	m_server.exit();

	return 0;
}

int main(int argc, const char * argv[])
{
	clan::SetupCore setup_core;
	clan::SetupDisplay setup_display;
	clan::SetupSWRender setup_swrender;
	clan::SetupSound setup_sound;
	clan::SoundOutput output(44100);
	clan::SetupNetwork setup_network;
	clan::ConsoleWindow console("Console", 160, 1000);
	clan::ConsoleLogger logger;

	clan::log_event("system", "Server starting.");

	std::vector<std::string> args;

	loopi(argc)
	{
		args.push_back(argv[i]);
	}

	ServerApp app;
	return app.main(args);
}
