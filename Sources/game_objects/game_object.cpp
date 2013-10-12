#include "precomp.h"
#include "game_object_manager.h"
#include "game_object.h"

uint32_t GameObject::unique_runtime_id_counter = 0;

GameObject::GameObject(uint32_t type, GameObjectManager * gom)
{
	m_game_object_manager = gom;
	m_type = type;
	m_id = 0;
	m_urid = unique_runtime_id_counter;
	unique_runtime_id_counter++;
}

GameObject::~GameObject()
{

}

clan::vec2 GameObject::get_pos()
{
	return m_pos;
}

uint32_t GameObject::get_type()
{
	return m_type;
}

uint32_t GameObject::get_id()
{
	return m_id;
}

uint32_t GameObject::get_urid()
{
	return m_urid;
}

void GameObject::set_pos(const clan::vec2 & pos)
{
	m_pos = pos;
}


void GameObject::set_id(uint32_t id)
{
	m_id = id;
}
