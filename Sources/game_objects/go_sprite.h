#pragma once
#include "game_object.h"

class GOSprite: public GameObject
{
protected:
	clan::Sprite m_sprite;
public:
	static GameObject * create(uint32_t guid){return new GOSprite(guid);};
	static uint32_t type(){return EGOT_SPRITE;}

	GOSprite(uint32_t guid);
	virtual ~GOSprite();

	void load(clan::ResourceManager & resources);

	virtual void update(const clan::GameTime & time);
	virtual void render(clan::Canvas & c, const clan::vec2 & offset);
};