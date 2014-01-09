#ifndef THROWABLE_OBJECT_H
#define THROWABLE_OBJECT_H

#include "game_object.h"
#include "game_object_types.h"

class ThrowableObject: public GameObject
{
protected:
	static clan::Sprite m_rock_sprite; ///preloaded sprites
	clan::Sprite m_sprite;

	clan::CollisionOutline m_outline;

	Property<clan::vec2f> m_vel;

	uint32_t m_time_spawned, m_owner_guid;
public:
	uint32_t get_spawn_time();
	uint32_t get_owner_guid();

	void init(uint32_t spawn_time, uint32_t owner_guid);

	Property<clan::vec2f>	get_vel();

public:
	static GameObject * create(uint32_t guid){return new ThrowableObject(guid);};
	static bool preload(clan::Canvas & canvas, clan::ResourceManager & resources);
	static void free();
	static uint32_t type(){return EGOT_THROWABLE_OBJECT;}

	ThrowableObject(uint32_t guid);
	virtual ~ThrowableObject();

	void load(clan::Canvas & canvas, clan::ResourceManager & resources);

	virtual void update(const clan::GameTime & time);
	virtual void render(clan::Canvas & c, const clan::vec2 & offset);
	virtual void on_message(const Message & msg);

	virtual void on_collide(GameObject * obj);
	virtual clan::CollisionOutline & get_outline();
};

#endif