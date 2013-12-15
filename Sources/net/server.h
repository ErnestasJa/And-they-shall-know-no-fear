#pragma once


class Player;
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
	Player**					m_player_objects;

	GameObjectManager*			m_gom;

	clan::NetGameServer			m_net_server;
	clan::SlotContainer			m_slots;

	clan::NetGameEventDispatcher_v1<ServerClientConnection*> m_auth_events, m_game_events, m_net_events, m_game_obj_sync_events;

	clan::GameTime m_game_time;
public:

	//connect
	void on_client_connected(clan::NetGameConnection *connection);
	void on_client_disconnected(clan::NetGameConnection *connection, const std::string &message);

public:
	//events
	void on_event		(clan::NetGameConnection *connection, const clan::NetGameEvent &e);
	void on_auth		(const clan::NetGameEvent &e, ServerClientConnection * user);
	void on_game_event	(const clan::NetGameEvent &e, ServerClientConnection * user);
	void on_game_object_sync_event(const clan::NetGameEvent &e, ServerClientConnection * user);
	void on_net_event	(const clan::NetGameEvent &e, ServerClientConnection * user);

public:
	void sync_game_objects(bool sync_only_changed_props = false);

public:
	bool init(uint32_t max_users, const std::string & port);
	bool run();
	void exit();

public:
	ServerClientConnection & get_client(uint32_t id);
	uint32_t get_max_clients();
};