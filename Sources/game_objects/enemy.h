#ifndef ENEMY_H
#define ENEMY_H

#include "game_object.h"
#include "game_object_types.h"

class Enemy: public GameObject
{
protected:
	static clan::Sprite m_bat_sprite; ///preloaded sprites
	clan::Sprite m_sprite;

	clan::CollisionOutline m_outline;

	Property<clan::vec2f> m_vel;

public:
	void init();
	Property<clan::vec2f>	get_vel();

public:
	static GameObject * create(uint32_t guid){return new Enemy(guid);};
	static bool preload(clan::Canvas & canvas, clan::ResourceManager & resources);
	static void free();
	static uint32_t type(){return EGOT_ENEMY;}

	Enemy(uint32_t guid);
	virtual ~Enemy();

	void load(clan::Canvas & canvas, clan::ResourceManager & resources);

	virtual void update(const clan::GameTime & time);
	virtual void render(clan::Canvas & c, const clan::vec2 & offset);
	virtual void on_message(const Message & msg);

	virtual void on_collide(GameObject * obj);
	virtual void on_tile_collide(const Tile & tile);
	virtual clan::CollisionOutline & get_outline();
};

#endif