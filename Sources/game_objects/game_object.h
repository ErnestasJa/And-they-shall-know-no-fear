#pragma once

class GameObjectManager;
class GameObject
{
protected:
	GameObjectManager * m_game_object_manager;
	uint32_t	m_type;
	uint32_t	m_id;
	uint32_t	m_urid;
	clan::vec2	m_pos;
	
	static uint32_t	unique_runtime_id_counter;

public:
	GameObject(uint32_t	type, GameObjectManager * gom);
	virtual ~GameObject();

	clan::vec2	get_pos();
	uint32_t	get_type();
	uint32_t	get_id();
	uint32_t	get_urid();

	void		set_id(uint32_t id);
	void		set_pos(const clan::vec2 & pos);

	virtual void load(clan::ResourceManager & resources)=0;

	virtual void update(const clan::GameTime & time)=0;
	virtual void render(clan::Canvas & c, const clan::vec2 & offset)=0;
};