#pragma once

class GameObject;
class GameObjectManager
{
protected:
	static uint32_t	globally_unique_id_counter;
	std::vector<GameObject*> m_game_object_list;

public:
	GameObjectManager();
	virtual ~GameObjectManager();

	///Creates game object and also adds it to internal list
	GameObject * add_game_object(uint32_t type, uint32_t guid);
	void remove_game_object(uint32_t guid);

	GameObject * find_game_object_by_id(uint32_t id);
	GameObject * find_game_object_by_guid(uint32_t id);

	std::vector<GameObject*> & get_game_objects();

	void update_game_objects(const clan::GameTime & game_time);
	void render_game_objects(clan::Canvas & canvas, const clan::vec2 & offset = clan::vec2(0,0));

	///should this really be here or on server code?
	uint32_t generate_guid();

///------Game object factory-------
protected:
	typedef GameObject * (*go_create_func)(uint32_t);
	std::map<uint32_t, go_create_func> m_go_create;

	GameObject * create_game_object(uint32_t type, uint32_t guid);
public:

	template <class T>
	bool register_game_object()
	{
		///galbut reiketu patikrinti ar nera jau priregistruoto objekto tokiu tipu
		m_go_create[T::type()]=&T::create;
		return true;
	}
};
