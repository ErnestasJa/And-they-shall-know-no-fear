#pragma once
#include "game_object.h"

class GOSprite: public GameObject
{
protected:
	clan::Sprite spr;
public:
	GOSprite();
	virtual ~GOSprite();

	virtual void update(const clan::GameTime & time);
	virtual void render(clan::Canvas & c, const clan::vec2 & offset);
};