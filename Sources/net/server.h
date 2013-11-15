#pragma once


class GOSprite;
class GameObjectManager;
class ServerClientConnection;
class Message;
class Client;
class ServerClientConnection;
class Server
{
protected:
	uint32_t					m_max_clients;
	ServerClientConnection*		m_client_cons;
	Client*						m_clients;
	GOSprite**					m_player_objects;

	GameObjectManager*			m_gom;

	clan::NetGameServer			m_net_server;
	clan::SlotContainer			m_slots;

	clan::NetGameEventDispatcher_v1<ServerClientConnection*> m_login_events;
	clan::NetGameEventDispatcher_v1<ServerClientConnection*> m_game_events;

	clan::GameTime m_game_time;
public:

	//connect
	void on_client_connected(clan::NetGameConnection *connection);
	void on_client_disconnected(clan::NetGameConnection *connection, const std::string &message);

public:
	//events
	void on_event(clan::NetGameConnection *connection, const clan::NetGameEvent &e);

	void on_auth(const clan::NetGameEvent &e, ServerClientConnection * user);

	void on_game_event(const clan::NetGameEvent &e, ServerClientConnection * user);

public:
	void create_all_game_objects(ServerClientConnection * client); /// implement this, then refactor the fuck out of everything
	void send_message(const Message & msg);
public:
	bool init(uint32_t max_users, const std::string & port);
	bool run();
	void exit();

public:
	ServerClientConnection & get_client(uint32_t id);
	uint32_t get_max_clients();
};