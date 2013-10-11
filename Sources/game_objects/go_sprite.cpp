#include "precomp.h"
#include "go_sprite.h"
#include "game_object_types.h"

GOSprite::GOSprite(): GameObject(GOT_SPRITE)
{

}

GOSprite::~GOSprite()
{

}

void GOSprite::load(clan::Sprite sprite)
{
	m_sprite = sprite;
	m_pos = clan::vec2(50,50);
	clan::Console::write_line("is null: %1",m_sprite.is_null());
}

void GOSprite::update(const clan::GameTime & time)
{
	m_pos.x += time.get_ticks_elapsed();
}

void GOSprite::render(clan::Canvas & c, const clan::vec2 & offset)
{
	m_sprite.draw(c, m_pos.x+offset.x, m_pos.y+offset.y);
}
