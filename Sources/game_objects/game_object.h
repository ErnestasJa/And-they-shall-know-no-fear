#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "properties/property.h"
#include "properties/property_container.h"

/**
Base game object class.

Derived classes should implement all pure virtual functions and have these functions:
	static GameObject * create(uint32_t guid){return new <ObjectType>(guid);};
**/

class Message;
class GameObjectManager;
struct Tile;
class GameObject: public PropertyContainer
{
protected:
	uint32_t	m_type;
	uint32_t	m_guid;
	bool		m_is_alive;

	///faster (direct) access to properties
	Property<uint32_t>		m_id;
	Property<clan::vec2f>	m_pos;

public:
	GameObject(uint32_t	type, uint32_t guid);
	virtual ~GameObject();

	///should these two variables also be properties?
	uint32_t	get_type();
	uint32_t	get_guid();

	Property<clan::vec2f>	get_pos();
	Property<uint32_t>		get_id();

	virtual void update(const clan::GameTime & time)=0;
	virtual void render(clan::Canvas & c, const clan::vec2 & offset)=0;
	virtual void on_message(const Message & msg)=0;

	virtual void on_collide(GameObject * obj) = 0;
	virtual void on_tile_collide(const Tile & tile) = 0;
	virtual clan::CollisionOutline & get_outline() = 0;

	///server only for now?
	virtual bool is_alive();
	virtual void set_is_alive(bool is_alive);
};

#endif
