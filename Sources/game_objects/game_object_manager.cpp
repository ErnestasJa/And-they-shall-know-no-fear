#include "precomp.h"
#include "game_objects.h"
#include "game_object_manager.h"

uint32_t GameObjectManager::globally_unique_id_counter = 0;

GameObjectManager::GameObjectManager()
{
	register_game_object<GOSprite>();
}

GameObjectManager::~GameObjectManager()
{

}

uint32_t GameObjectManager::generate_guid()
{
	const uint32_t guid = GameObjectManager::globally_unique_id_counter;
	GameObjectManager::globally_unique_id_counter++;
	return guid;
}

std::vector<GameObject*> & GameObjectManager::get_game_objects()
{
	return m_game_object_list;
}

GameObject * GameObjectManager::add_game_object(uint32_t type, uint32_t guid)
{
	GameObject * o = create_game_object(type,guid);
	m_game_object_list.push_back(o);
	return o;
}

void GameObjectManager::remove_game_object(uint32_t guid)
{
	auto it = std::find_if(m_game_object_list.begin(), m_game_object_list.end(), [&guid](GameObject * o){return o->get_guid()==guid;});

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


GameObject * GameObjectManager::find_game_object_by_guid(uint32_t guid)
{
	auto it = std::find_if(m_game_object_list.begin(), m_game_object_list.end(), [&guid](GameObject * o){return o->get_guid()==guid;});

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

///factory

GameObject * GameObjectManager::create_game_object(uint32_t type, uint32_t guid)
{
	auto it = m_go_create.find(type);

	if(it != m_go_create.end())
		return it->second(guid);

	throw clan::Exception("Cannot create object of type " + clan::StringHelp::uint_to_text(type));
}