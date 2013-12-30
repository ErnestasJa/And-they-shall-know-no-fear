#include "precomp.h"
#include "net/message.h"
#include "game_object_manager.h"
#include "game_object.h"


GameObject::GameObject(uint32_t type, uint32_t guid)
{
	m_type = type;
	m_guid = guid;

	///must init properties
	m_id = add_property<uint32_t>("id",0);
	m_pos = add_property<clan::vec2f>("pos",clan::vec2f(0,0));
}

GameObject::~GameObject()
{

}

uint32_t GameObject::get_type()
{
	return m_type;
}

uint32_t GameObject::get_guid()
{
	return m_guid;
}

///property getters
Property<clan::vec2f> GameObject::get_pos()
{
	return m_pos;
}

Property<uint32_t> GameObject::get_id()
{
	return m_id;
}