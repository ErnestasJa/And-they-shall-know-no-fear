#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H

#include "tile_map.h"

class GameObject;
class Message;
class Player;

class GameObjectManager
{
protected:
	std::vector<GameObject*> m_game_object_list;
	std::vector<GameObject*> m_tmp_object_list;

	clan::Callback_v1<GameObject*> m_on_add_game_object;
	clan::Callback_v1<GameObject*> m_on_remove_game_object;
	clan::Callback_v1<GameObject*> m_on_update_game_object;

	TileMap m_tile_map;

#if defined GAME_SERVER
	clan::CollisionOutline m_tile_outline;
#endif

public:
	GameObjectManager(TileMap map);
	virtual ~GameObjectManager();

	void preload(clan::Canvas & canvas ,clan::ResourceManager & resources);
	void free();

	///Creates game object and also adds it to internal list
	GameObject * add_game_object(uint32_t type, uint32_t guid);
	void remove_game_object(uint32_t guid);

	GameObject * find_game_object_by_id(uint32_t id);
	GameObject * find_game_object_by_guid(uint32_t guid);

	std::vector<GameObject*> & get_game_objects();
	std::vector<GameObject*> & get_tmp_game_objects();

	void update_game_objects(const clan::GameTime & game_time);
	void collide_game_objects(const clan::GameTime & game_time);
	void render_game_objects(clan::Canvas & canvas, const clan::vec2 & offset = clan::vec2(0,0));
	void remove_not_alive_objects();

	void on_net_event(const clan::NetGameEvent & e);
	void on_game_object_sync_event(const clan::NetGameEvent & e);

///------Callbacks for server and ... --------
public:
	clan::Callback_v1<GameObject*> & func_on_add_game_object();
	clan::Callback_v1<GameObject*> & func_on_remove_game_object();
	clan::Callback_v1<GameObject*> & func_on_update_game_object();

///------Game object factory-------
protected:
	typedef GameObject * (*go_create_func)(uint32_t);
	typedef bool (*go_preload_func)(clan::Canvas & canvas ,clan::ResourceManager & resources);
	typedef void (*go_free_func)();

	struct go_funcs
	{
		go_create_func	gc;
		go_preload_func gp;
		go_free_func	gf;

		go_funcs(){}

		go_funcs(go_create_func	gc, go_preload_func gp, go_free_func gf)
		{
			this->gc = gc;
			this->gp = gp;
			this->gf = gf;
		}
	};

	std::map<uint32_t, go_funcs > m_go_create;
	GameObject * create_game_object(uint32_t type, uint32_t guid);

public:
	template <class T>
	bool register_game_object()
	{
		m_go_create[T::type()]=go_funcs(&T::create,&T::preload,&T::free);
		return true;
	}
};

#endif
