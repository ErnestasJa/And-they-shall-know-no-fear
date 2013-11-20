#pragma once

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

	void send_message(const Message & msg, bool only_changed = false);

public:
	///event
	clan::Signal_v1< const clan::NetGameEvent & > & sig_event_received();

public:
};