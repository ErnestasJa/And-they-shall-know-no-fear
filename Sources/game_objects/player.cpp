#include "precomp.h"
#include "game_object_types.h"
#include "game_object_manager.h"
#include "game_object.h"
#include "player.h"
#include "../tile_chunk.h"
#include "throwable_object.h"
#include "../net/message.h"

clan::Sprite Player::s_rw, 
			 Player::s_lw, 
			 Player::s_uw, 
			 Player::s_dw,
			 Player::s_h;

static clan::Font title;

Player::Player(uint32_t guid): GameObject(type(),guid)
{
	#if defined GAME_SERVER
		m_gom = nullptr;
	#endif

	keys=add_property<uint32_t>("keys",0);
	life=add_property<uint32_t>("life",100);
	mana=add_property<float>("mana",50);
	kills=add_property<uint32_t>("kills",0);
	killer=add_property<uint32_t>("killer");
	name=add_property<std::string>("name","Sir. Waitfor Servereply");

	clan::Contour contour;
	contour.get_points().push_back(clan::Pointf(16,13));
	contour.get_points().push_back(clan::Pointf(16,63));
	contour.get_points().push_back(clan::Pointf(47,63));
	contour.get_points().push_back(clan::Pointf(47,13));

	m_outline.get_contours().push_back(contour);
	m_outline.calculate_radius();
	m_outline.calculate_smallest_enclosing_discs();
	
	m_next_attack_time = 0;
	m_vel = 48.0f;
}

Player::~Player()
{

}

uint32_t Player::get_next_attack_time()
{
	return m_next_attack_time;
}

void Player::set_next_attack_time(uint32_t time)
{
	m_next_attack_time = time;
}

clan::Colorf Player::get_hp_bar_color()
{
	float r,g;
	r = 1.0f - (float)life/100;
	g = (float)life/100;
	return clan::Colorf(r,g,0.0f,1.0f);
}

bool Player::preload(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	title = clan::Font(canvas,"Arial",12); ///DON'T CHANGE EVEN AGAIN
	s_rw = clan::Sprite::resource(canvas, "champ_rw", resources );
	s_lw = clan::Sprite::resource(canvas, "champ_lw", resources );
	s_uw = clan::Sprite::resource(canvas, "champ_uw", resources );
	s_dw = clan::Sprite::resource(canvas, "champ_dw", resources );

	s_h = clan::Sprite::resource(canvas, "hp_bar", resources );

	return true;
}

void Player::free()
{
	s_rw = clan::Sprite();
	s_lw = clan::Sprite();
	s_uw = clan::Sprite();
	s_dw = clan::Sprite();

	s_h = clan::Sprite();

	title = clan::Font();
}

void Player::init()
{
	m_rw = s_rw.clone();
	m_lw = s_lw.clone();
	m_uw = s_uw.clone();
	m_dw = s_dw.clone();

	m_h = s_h.clone();

	m_sprite=m_dw;
}

#if defined GAME_SERVER
void Player::init_server(GameObjectManager * gom)
{
	m_gom = gom;
}
#endif

void Player::update(const clan::GameTime & time)
{
	last_pos = m_pos.get();

#if defined GAME_CLIENT
	m_sprite.update(time.get_time_elapsed_ms());

	if(keys&EUIKT_MOVE_LEFT)
		m_sprite=m_lw;
	else if(keys&EUIKT_MOVE_RIGHT)
		m_sprite=m_rw;
	else if(keys&EUIKT_MOVE_UP)
		m_sprite=m_uw;
	if(keys&EUIKT_MOVE_DOWN)
		m_sprite=m_dw;
#endif

#if defined GAME_SERVER

	float delta_time = (float)time.get_time_elapsed_ms()/1000.0f;

	float mana_degen = -10.0f;

	float speed = m_vel + ((100-life)/10.0f)*2.0f;

	if((keys&EUIKT_RUN) && (mana > -1.0f * (mana_degen * delta_time)))
		speed += 20.0f;

	speed *= delta_time;

	clan::vec2f mvel;

	if(keys&EUIKT_MOVE_LEFT && m_pos.get().x>-20.0f)
	{
		mvel.x-= 1;
	}
	if(keys&EUIKT_MOVE_RIGHT && m_pos.get().x<980.0f)
	{
		mvel.x+= 1;
	}
	if(keys&EUIKT_MOVE_UP && m_pos.get().y>0.0f)
	{
		mvel.y-= 1;
	}
	if(keys&EUIKT_MOVE_DOWN && m_pos.get().y<660.0f)
	{
		mvel.y+= 1;
	}

	if(keys&EUIKT_MOVE_LEFT || keys&EUIKT_MOVE_RIGHT || keys&EUIKT_MOVE_UP || keys&EUIKT_MOVE_DOWN)
	{
		mvel = mvel.normalize();
		mvel *= speed;
		m_pos.set(m_pos.get() + mvel);
	}

	if(keys&EUIKT_ATTACK)
	{
		if(get_next_attack_time()<=time.get_current_time_ms())
		{
			///spawn rock, shoot rock
			ThrowableObject * obj = static_cast<ThrowableObject *>(m_gom->add_game_object(EGOT_THROWABLE_OBJECT, m_gom->generate_guid()));
			obj->init(time.get_current_time_ms(),get_guid());

			clan::vec2f vel;
			vel.x = (keys & EUIKT_MOVE_LEFT ? - 1 : (keys & EUIKT_MOVE_RIGHT ? 1 : 0 ) );
			vel.y = (keys & EUIKT_MOVE_UP ? - 1 : (keys & EUIKT_MOVE_DOWN ? 1 : 0 ) );
			vel = vel.normalize();

			vel *= mana*2.5f;

			if(mana>=20)
				mana = mana - 20;
			else
				mana = 0;	

			clan::vec2f off;
			off.x = 26;
			off.y = 28;

			obj->get_vel().set(vel);
			obj->get_pos().set(get_pos().get()+off);
			set_next_attack_time(time.get_current_time_ms() + 600);
		}
	}

	float mana_regen = 0.0f;

	if(keys&EUIKT_MOVE_LEFT || keys&EUIKT_MOVE_RIGHT || keys&EUIKT_MOVE_UP || keys&EUIKT_MOVE_DOWN)
	{
		if(keys&EUIKT_RUN)
			mana_regen = mana_degen;
		else
			mana_regen = 10.0f;
	}
	else
		mana_regen = 16.0f;

	mana = mana + (mana_regen*delta_time);
	
	if(mana>100.0f)mana=100.0f;
	else if(mana<0.0f) mana=0.0f;

#endif

	m_outline.set_translation(m_pos.get().x,m_pos.get().y);
}

void Player::render(clan::Canvas & c, const clan::vec2 & offset)
{
#if defined GAME_CLIENT
	if(!m_sprite.is_null())
	{
		m_sprite.draw(c, m_pos.get().x+offset.x, m_pos.get().y+offset.y);
		c.fill_rect(m_pos.get().x+offset.x+10, m_pos.get().y+offset.y-1,  m_pos.get().x+offset.x+10+(44*life/100), m_pos.get().y+offset.y+4,  get_hp_bar_color());
		c.fill_rect(m_pos.get().x+offset.x+10, m_pos.get().y+offset.y+4,  m_pos.get().x+offset.x+10+(44*mana/100), m_pos.get().y+offset.y+8,  clan::Colorf(0.5f,0.5f,1.0f,1.0f));
		m_h.draw(c,m_pos.get().x+offset.x, m_pos.get().y+offset.y-5);
		m_outline.draw(offset.x,offset.y,clan::Colorf(1,0,0,1),c);

		title.draw_text(c, m_pos.get().x+offset.x+6,m_pos.get().y+offset.y-8, name.get()+": "+clan::StringHelp::uint_to_text(kills.get()), get_hp_bar_color());
	}
#endif
}

void Player::on_message(const Message & msg)
{
	if(msg.get_type()==MSGC_INPUT)
	{
		const MSGC_Input & input = static_cast<const MSGC_Input &>(msg);
		keys = input.keys;
	}
}

void Player::on_collide(GameObject * obj)
{
	if(obj->is_alive() && obj->get_type()==EGOT_THROWABLE_OBJECT)
	{
		if(static_cast<ThrowableObject*>(obj)->get_owner_guid()==this->get_guid())
			return;

		obj->set_is_alive(false);

		if(this->life>10)
		{
			this->life = this->life - 10;
		}
		else
		{
			this->killer=static_cast<ThrowableObject*>(obj)->get_owner_guid();
			this->life=0;
		}
	}
}

void Player::on_tile_collide(const Tile & tile)
{
	m_pos = last_pos;
}

clan::CollisionOutline & Player::get_outline()
{
	return m_outline;
}

Property<float> Player::get_mana()
{
	return mana;
}