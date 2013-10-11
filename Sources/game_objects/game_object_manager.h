#pragma once

class GameObject;

class GameObjectManager
{
protected:
	std::vector<GameObject*> m_game_object_list;
	clan::GameTime m_game_time;
public:
	GameObjectManager();
	virtual ~GameObjectManager();

	void add_game_object(GameObject * o);
	void remove_game_object(GameObject * o);

	GameObject * find_game_object_by_id(uint32_t id);

	void update_game_objects();
	void render_game_objects(clan::Canvas & canvas, const clan::vec2 & offset = clan::vec2(0,0));
};