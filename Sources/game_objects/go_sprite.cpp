#include "precomp.h"
#include "go_sprite.h"


GOSprite::GOSprite(): GameObject()
{

}

GOSprite::~GOSprite()
{

}

void GOSprite::load(clan::Sprite sprite)
{
	m_sprite = sprite;
}

void GOSprite::update(const clan::GameTime & time)
{

}

void GOSprite::render(clan::Canvas & c, const clan::vec2 & offset)
{

}
