#pragma once

#include "tile_map.h"

#include "properties/property_container.h"
#include "properties/property.h"

class Player;
class GameObjectManager;
class ServerClientConnection;
class Message;
class Client;
class ServerClientConnection;
class GameObject;
class Server: public PropertyContainer
{
protected:

	
	Property<uint32_t>			m_max_clients;
	Property<std::string>		m_port;
	Property<std::string>		m_map;

	uint32_t					m_current_guid;
	ServerClientConnection*		m_client_cons;
	Client*						m_clients;
	Player**					m_player_objects;

	GameObjectManager*			m_gom;
	TileMap						m_tile_map;

	clan::NetGameServer			m_net_server;
	clan::SlotContainer			m_slots;

	clan::Slot m_on_update_game_object;

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

	void send_events(const clan::NetGameEvent &e, uint32_t client_flag);

public:
	void sync_game_objects(bool sync_only_changed_props = false);
	void send_game_objects(ServerClientConnection * cl, bool sync_only_changed_props = false);

public:
	void on_update_game_object(GameObject * obj);
	void on_add_game_object(GameObject * obj);
	void on_remove_game_object(GameObject * obj);

public:
	void spawn_client(Client * client);

public:
	bool init();
	void init_default();
	bool run();
	void exit();

public:
	
	ServerClientConnection & get_client(uint32_t id);
	uint32_t get_max_clients();
};