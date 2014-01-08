#include "precomp.h"
#include "game_object_types.h"
#include "game_object_manager.h"
#include "game_object.h"
#include "throwable_object.h"
#include "../net/message.h"

clan::Sprite ThrowableObject::m_rock_sprite;

ThrowableObject::ThrowableObject(uint32_t guid): GameObject(type(),guid)
{
	m_vel = add_property("velocity", clan::vec2f(0,0));

	clan::Contour contour;
	contour.get_points().push_back(clan::Pointf(0,0));
	contour.get_points().push_back(clan::Pointf(0,12));
	contour.get_points().push_back(clan::Pointf(12,12));
	contour.get_points().push_back(clan::Pointf(12,0));

	m_outline.get_contours().push_back(contour);
	m_outline.calculate_radius();
	m_outline.calculate_smallest_enclosing_discs();
}

ThrowableObject::~ThrowableObject()
{

}

uint32_t ThrowableObject::get_spawn_time()
{
	return m_time_spawned;
}

void ThrowableObject::set_spawn_time(uint32_t time)
{
	m_time_spawned = time;
}

Property<clan::vec2f> ThrowableObject::get_vel()
{
	return m_vel;
}

bool ThrowableObject::preload(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	m_rock_sprite = clan::Sprite::resource(canvas, "rock", resources );
	
	return true;
}

void ThrowableObject::free()
{
	m_rock_sprite = clan::Sprite();
}

void ThrowableObject::load(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	m_sprite=m_rock_sprite;
}

void ThrowableObject::update(const clan::GameTime & time)
{
	if(!m_sprite.is_null())
		m_sprite.update(time.get_time_elapsed_ms());

	clan::vec2f v = m_vel.get() * clan::vec2f((float)time.get_time_elapsed_ms()/1000.0f,(float)time.get_time_elapsed_ms()/1000.0f);
	m_pos.set(m_pos.get()+v);

	m_outline.set_translation(m_pos.get().x,m_pos.get().y);
}

void ThrowableObject::render(clan::Canvas & c, const clan::vec2 & offset)
{
	if(!m_sprite.is_null())
	{
		m_sprite.draw(c, m_pos.get().x+offset.x, m_pos.get().y+offset.y);
		m_outline.draw(offset.x,offset.y,clan::Colorf(1,0,0,1),c);
	}
}

void ThrowableObject::on_message(const Message & msg)
{
	
}

void ThrowableObject::on_collide(GameObject * obj)
{

}

clan::CollisionOutline & ThrowableObject::get_outline()
{
	return m_outline;
}