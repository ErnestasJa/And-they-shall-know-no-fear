#pragma once

class ServerClient
{
protected:
	clan::NetGameConnection * m_connection;

	uint32_t	m_id;
	uint32_t	m_flags;
	std::string m_name;
public:

	ServerClient();
	~ServerClient();

	void init(uint32_t id);
	uint32_t get_id();

	void clear_flag(uint32_t flag);
	bool check_flag(uint32_t flag);
	void set_flag(uint32_t flag);

	void set_name(const std::string & name);
	const std::string & get_name();
public:
	static ServerClient * get_client(clan::NetGameConnection * connection);
	
	void connect(clan::NetGameConnection * connection);
	clan::NetGameConnection * get_connection();
	bool is_connected();
	void disconnect();
};