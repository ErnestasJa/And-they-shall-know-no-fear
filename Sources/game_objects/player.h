#ifndef PLAYER_H
#define PLAYER_H

#include "game_object.h"
#include "game_object_types.h"

class Player: public GameObject
{
protected:
	uint32_t m_next_attack_time;

public:
	uint32_t get_next_attack_time();
	void set_next_attack_time(uint32_t time);
	clan::Colorf get_hp_bar_color();

protected:
	static clan::Sprite s_rw,s_lw,s_uw,s_dw, s_h; ///preloaded sprites
	clan::Sprite m_rw,m_lw,m_uw,m_dw, m_h; ///instanced sprites
	clan::Sprite m_sprite; ///current sprite

	Property<uint32_t> keys, life;
	Property<std::string> name;
	Property<uint32_t> vel;

	clan::CollisionOutline m_outline;
public:
	static GameObject * create(uint32_t guid){return new Player(guid);};
	static bool preload(clan::Canvas & canvas, clan::ResourceManager & resources);
	static void free();
	static uint32_t type(){return EGOT_PLAYER;}

	Player(uint32_t guid);
	virtual ~Player();

	void load(clan::Canvas & canvas, clan::ResourceManager & resources);

	virtual void update(const clan::GameTime & time);
	virtual void render(clan::Canvas & c, const clan::vec2 & offset);
	virtual void on_message(const Message & msg);

	virtual void on_collide(GameObject * obj);
	virtual clan::CollisionOutline & get_outline();
};

#endif
