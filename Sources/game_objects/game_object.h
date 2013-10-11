#pragma once

class GameObject
{
protected:
	uint32_t	m_type;
	uint32_t	m_id;
	uint32_t	m_urid;
	clan::vec2	m_pos;
	

	static uint32_t	unique_runtime_id_counter;

public:
	GameObject(uint32_t	type);
	virtual ~GameObject();

	clan::vec2	get_pos();
	uint32_t	get_type();
	uint32_t	get_id();
	uint32_t	get_urid();

	void		set_id(uint32_t id);
	void		set_pos(const clan::vec2 & pos);

	virtual void update(const clan::GameTime & time)=0;
	virtual void render(clan::Canvas & c, const clan::vec2 & offset)=0;
};