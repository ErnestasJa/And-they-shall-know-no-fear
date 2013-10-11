#pragma once
#include "game_object.h"

class GOSprite: public GameObject
{
protected:
	clan::Sprite m_sprite;
public:
	GOSprite();
	virtual ~GOSprite();

	void load(clan::Sprite sprite);

	virtual void update(const clan::GameTime & time);
	virtual void render(clan::Canvas & c, const clan::vec2 & offset);
};