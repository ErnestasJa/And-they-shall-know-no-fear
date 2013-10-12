#pragma once
#include "game_object.h"

class GOSprite: public GameObject
{
protected:
	clan::Sprite m_sprite;
public:
	GOSprite(GameObjectManager * gom);
	virtual ~GOSprite();

	void load(clan::ResourceManager & resources);

	virtual void update(const clan::GameTime & time);
	virtual void render(clan::Canvas & c, const clan::vec2 & offset);
};