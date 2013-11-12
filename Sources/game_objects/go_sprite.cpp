#include "precomp.h"
#include "game_object_types.h"
#include "game_object_manager.h"
#include "game_objects.h"


GOSprite::GOSprite(uint32_t guid): GameObject(type(),guid)
{
	keys=0;
}

GOSprite::~GOSprite()
{

}

void GOSprite::load(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	m_rw = clan::Sprite::resource(canvas, "champ_rw", resources );
	m_lw = clan::Sprite::resource(canvas, "champ_lw", resources );
	m_uw = clan::Sprite::resource(canvas, "champ_uw", resources );
	m_dw = clan::Sprite::resource(canvas, "champ_dw", resources );

	m_sprite=m_dw;

	clan::Console::write_line("is null: %1",m_sprite.is_null());
}

void GOSprite::update(const clan::GameTime & time)
{
	m_sprite.update(time.get_time_elapsed_ms());
	//m_pos.data().x += 16.0f * (float)time.get_time_elapsed_ms()/1000.0f;

	if(CHECK_BIT(keys,EUIKT_MOVE_LEFT))
	{
		m_sprite=m_lw;
		m_pos.data().x -= 32.0f * (float)time.get_time_elapsed_ms()/900.0f;
	}
	if(CHECK_BIT(keys,EUIKT_MOVE_RIGHT))
	{
		m_sprite=m_rw;
		m_pos.data().x += 32.0f * (float)time.get_time_elapsed_ms()/900.0f;
	}
	if(CHECK_BIT(keys,EUIKT_MOVE_UP))
	{
		m_sprite=m_uw;
		m_pos.data().y -= 32.0f * (float)time.get_time_elapsed_ms()/900.0f;
	}
	if(CHECK_BIT(keys,EUIKT_MOVE_DOWN))
	{
		m_sprite=m_dw;
		m_pos.data().y += 32.0f * (float)time.get_time_elapsed_ms()/900.0f;
	}
}

void GOSprite::render(clan::Canvas & c, const clan::vec2 & offset)
{
	m_sprite.draw(c, m_pos.data().x+offset.x, m_pos.data().y+offset.y);
}

void GOSprite::on_message(const Message & msg)
{
	if(msg.get_type()==MSGC_INPUT)
	{
		const MSGC_Input & input = (const MSGC_Input &)msg;
		keys = input.keys;
	}
}