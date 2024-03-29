#ifndef USER_CLIENT_H
#define USER_CLIENT_H

#include "common.h"

class ClientConnection
{
protected:
	clan::NetGameClient m_net_client;
	clan::SlotContainer m_slots;

public:

	void connect(const std::string & server,const std::string & port);
	void disconnect();

	clan::Signal_v0 & sig_connected();
	clan::Signal_v0 & sig_disconnected();

	void send_event(const clan::NetGameEvent & e);

public:
	///event
	clan::Signal_v1< const clan::NetGameEvent & > & sig_event_received();

public:
};

#endif
