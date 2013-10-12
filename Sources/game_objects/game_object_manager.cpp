#include "precomp.h"
#include "game_object.h"
#include "game_object_manager.h"

GameObjectManager::GameObjectManager(clan::Canvas & c)
{
	m_canvas = c;
}

GameObjectManager::~GameObjectManager()
{

}

clan::Canvas & GameObjectManager::get_canvas()
{
	return m_canvas;
}

void GameObjectManager::add_game_object(GameObject * o)
{
	m_game_object_list.push_back(o);
}

void GameObjectManager::remove_game_object(GameObject * o)
{
	auto it = std::find(m_game_object_list.begin(), m_game_object_list.end(), o);
	if(it!=m_game_object_list.end())
		m_game_object_list.erase(it);
}

GameObject * GameObjectManager::find_game_object_by_id(uint32_t id)
{
	auto it = std::find_if(m_game_object_list.begin(), m_game_object_list.end(), [&id](GameObject * o){return o->get_id()==id;});

	if(it==m_game_object_list.end())
		return nullptr;

	return (*it);
}

void GameObjectManager::update_game_objects(const clan::GameTime & game_time)
{
	for(auto it = m_game_object_list.begin(); it!=m_game_object_list.end(); it++)
		(*it)->update(game_time);
}

void GameObjectManager::render_game_objects(clan::Canvas & canvas, const clan::vec2 & offset)
{
	for(auto it = m_game_object_list.begin(); it!=m_game_object_list.end(); it++)
		(*it)->render(canvas,offset);
}