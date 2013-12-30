#pragma once

#include "state.h"
#include "tile_map.h"
#include "con_info.h"
#include "net\message.h"
#include "game_objects\game_objects.h"
#include "game_objects\game_object_manager.h"

class Client;
class ClientConnection;
class World: public State
{
public:
	World(clan::DisplayWindow &display_window);
	~World();

	bool init();
	bool run();
	bool exit();

	bool pause();
	bool resume();

	void set_info(const ConnectionInfo &);//DEBUG 

protected:
	///event funcs
	void on_key_up(const clan::InputEvent & e);
	void on_key_down(const clan::InputEvent & e);

	///game specific funcs
	void init_level(const std::string & level);

protected:
	///net
	clan::SlotContainer m_net_slots;

	void on_connected();
	void on_event(const clan::NetGameEvent & e);

	void on_auth_event		(const clan::NetGameEvent &e);
	void on_game_event	(const clan::NetGameEvent &e);
	void on_game_object_sync_event(const clan::NetGameEvent &e);
	void on_net_event	(const clan::NetGameEvent &e);

	void on_disconnected();

protected:
	bool m_run;
	clan::Slot m_key_up;
	clan::Slot m_key_down;

	clan::ResourceManager	m_resources;
	clan::DisplayWindow		m_window;
	clan::Canvas			m_canvas;
	clan::GameTime			m_game_time;

	///Kameros pozicija
	clan::vec2 m_pos;

	///game
	TileMap m_tile_map;
	
	MSGC_Input msg;
	MSGC_Auth msg_auth;

	uint32_t	m_client_id;
	std::string	m_server_ip, m_server_port, m_server_pass, m_client_name;

	ConnectionInfo	m_con_info;
	uint32_t	m_max_clients;

	GameObjectManager * m_gom;
	ClientConnection * m_client_con;
	clan::NetGameEventDispatcher_v0 m_auth_events, m_game_events, m_game_obj_sync_events, m_net_events;

	Client * m_client;
	Client * m_clients;
	Player * m_player;
	Player ** m_players;

	clan::NetGameEvent * m_game_event;
};
