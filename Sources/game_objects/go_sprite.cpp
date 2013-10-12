#include "precomp.h"
#include "game_object_types.h"
#include "game_object_manager.h"
#include "game_objects.h"


GOSprite::GOSprite(GameObjectManager * gom): GameObject(GOT_SPRITE,gom)
{

}

GOSprite::~GOSprite()
{

}

void GOSprite::load(clan::ResourceManager & resources)
{
	m_sprite = clan::Sprite::resource(m_game_object_manager->get_canvas(), "champ_rw", resources );
	clan::Console::write_line("is null: %1",m_sprite.is_null());
}

void GOSprite::update(const clan::GameTime & time)
{
	m_sprite.update (time.get_time_elapsed_ms());
	m_pos.x += time.get_ticks_elapsed();
}

void GOSprite::render(clan::Canvas & c, const clan::vec2 & offset)
{
	m_sprite.draw(c, m_pos.x+offset.x, m_pos.y+offset.y);
}