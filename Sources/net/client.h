#pragma once

class Client
{
protected:
	clan::NetGameClient m_net_client;
	clan::SlotContainer m_slots;

	clan::NetGameEventDispatcher_v0 m_login_events;
	clan::NetGameEventDispatcher_v0 m_game_events;
protected:

	uint32_t m_flags;
	uint32_t m_id;
	std::string m_name;

public:
	void connect(const std::string & server,const std::string & port);
	void disconnect();

	clan::Signal_v0 & sig_connected();
	clan::Signal_v0 & sig_disconnected();

	void clear_flag(uint32_t flag);
	bool check_flag(uint32_t flag);
	void set_flag(uint32_t flag);

	uint32_t get_id();
	void set_id(uint32_t id);

	void set_name(const std::string & name);
	const std::string & get_name();

	void send_message(const Message & msg);



public:
	///event
	clan::Signal_v1< const clan::NetGameEvent & > & sig_event_received();

public:
};