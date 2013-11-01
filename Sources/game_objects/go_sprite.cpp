#include "precomp.h"
#include "game_object_types.h"
#include "game_object_manager.h"
#include "game_objects.h"


GOSprite::GOSprite(uint32_t guid): GameObject(type(),guid)
{

}

GOSprite::~GOSprite()
{

}

void GOSprite::load(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	m_sprite = clan::Sprite::resource(canvas, "champ_rw", resources );
	clan::Console::write_line("is null: %1",m_sprite.is_null());
}

void GOSprite::update(const clan::GameTime & time)
{
	m_sprite.update(time.get_time_elapsed_ms());
	m_pos.data().x += 100 * (float)time.get_time_elapsed_ms()/1000.0f;
}

void GOSprite::render(clan::Canvas & c, const clan::vec2 & offset)
{
	m_sprite.draw(c, m_pos.data().x+offset.x, m_pos.data().y+offset.y);
}