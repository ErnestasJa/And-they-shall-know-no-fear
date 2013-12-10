#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H

class GameObject;
class Message;
class GameObjectManager
{
protected:
	std::vector<GameObject*> m_game_object_list;

public:
	GameObjectManager();
	virtual ~GameObjectManager();

	///Creates game object and also adds it to internal list
	GameObject * add_game_object(uint32_t type, uint32_t guid);
	void remove_game_object(uint32_t guid);

	GameObject * find_game_object_by_id(uint32_t id);
	GameObject * find_game_object_by_guid(uint32_t guid);

	std::vector<GameObject*> & get_game_objects();

	void update_game_objects(const clan::GameTime & game_time);
	void render_game_objects(clan::Canvas & canvas, const clan::vec2 & offset = clan::vec2(0,0));

	void on_message(const Message & msg);

///------Game object factory-------
protected:
	typedef GameObject * (*go_create_func)(uint32_t);
	typedef bool (*go_preload_func)(clan::Canvas & canvas ,clan::ResourceManager & resources);
	std::map<uint32_t, std::pair<go_create_func, go_preload_func> > m_go_create;
	GameObject * create_game_object(uint32_t type, uint32_t guid);

public:
	template <class T>
	bool register_game_object()
	{
		m_go_create[T::type()]=std::make_pair(&T::create,&T::preload);
		return true;
	}

};

#endif
