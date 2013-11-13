#pragma once


class GOSprite;
class GameObjectManager;
class ServerClient;
class Server
{
protected:
	ServerClient*		m_clients;
	GameObjectManager*	m_gom;
	GOSprite**			m_player_objects;
	uint32_t			m_max_clients;

	clan::NetGameServer m_net_server;
	clan::SlotContainer m_slots;

	clan::NetGameEventDispatcher_v1<ServerClient*> m_login_events;
	clan::NetGameEventDispatcher_v1<ServerClient*> m_game_events;

	clan::GameTime m_game_time;


public:

	//connect
	void on_client_connected(clan::NetGameConnection *connection);
	void on_client_disconnected(clan::NetGameConnection *connection, const std::string &message);

public:

	//events
	void on_event(clan::NetGameConnection *connection, const clan::NetGameEvent &e);

	void on_auth(const clan::NetGameEvent &e, ServerClient * user);

	void on_game_event(const clan::NetGameEvent &e, ServerClient * user);

public:
	void create_all_game_objects(ServerClient * client); /// implement this, then refactor the fuck out of everything

public:
	bool init(uint32_t max_users, const std::string & port);

	ServerClient & get_client(uint32_t id);
	uint32_t get_max_clients();

	bool run();

	void exit();
};