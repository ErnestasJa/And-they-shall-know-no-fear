#pragma once

class Client;
class Message;
class ServerClientConnection
{
protected:
	clan::NetGameConnection *	m_connection;
	Client *					m_client;
public:
	ServerClientConnection();
	~ServerClientConnection();

	void init(Client * client);
	Client * get_client();

public:
	static ServerClientConnection * get_client(clan::NetGameConnection * connection);

	void send_message(const Message & msg, bool only_changed = true);
	void send_event(const clan::NetGameEvent & e);
	void connect(clan::NetGameConnection * connection);
	clan::NetGameConnection * get_connection();
	bool is_connected();
	void disconnect();
};