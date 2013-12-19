#include "precomp.h"
#include "game_objects.h"
#include "game_object_manager.h"
#include "../net/message.h"

GameObjectManager::GameObjectManager()
{
	register_game_object<Player>();
}

GameObjectManager::~GameObjectManager()
{
	free();
}

void GameObjectManager::preload(clan::Canvas & canvas ,clan::ResourceManager & resources)
{
	for(auto it = m_go_create.begin(); it != m_go_create.end(); it++)
	{
		(*it).second.gp(canvas,resources);
	}
}

void GameObjectManager::free()
{
	for(auto it = m_go_create.begin(); it != m_go_create.end(); it++)
	{
		(*it).second.gf();
	}
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

void GameObjectManager::on_net_event(const clan::NetGameEvent & e)
{
	uint32_t type = e.get_argument(0).to_uinteger();
	clan::log_event("net_event","Got message from server type='%1' msg='%2'.",type,e.to_string());

	if(type==MSGC_INPUT)
	{
		MSGC_Input m;
		m.net_deserialize(e.get_argument(1));
		find_game_object_by_guid(m.id)->on_message(m);
	}

	///messages that are only handled on server
	#if defined GAME_SERVER

	#endif

	///messages that are only handled on client
	#if defined GAME_CLIENT

	#endif
}

void GameObjectManager::on_game_object_sync_event(const clan::NetGameEvent & e)
{

}

///factory
GameObject * GameObjectManager::create_game_object(uint32_t type, uint32_t guid)
{
	auto it = m_go_create.find(type);

	if(it != m_go_create.end())
		return it->second.gc(guid);

	throw clan::Exception("Cannot create object of type " + clan::StringHelp::uint_to_text(type));
}