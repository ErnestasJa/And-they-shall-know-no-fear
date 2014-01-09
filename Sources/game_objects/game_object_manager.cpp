#include "precomp.h"
#include "game_objects.h"
#include "game_object_manager.h"
#include "../net/message.h"

GameObjectManager::GameObjectManager()
{
	register_game_object<Player>();
	register_game_object<ThrowableObject>();
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


std::vector<GameObject*> & GameObjectManager::get_tmp_game_objects()
{
	return m_tmp_object_list;
}

GameObject * GameObjectManager::add_game_object(uint32_t type, uint32_t guid)
{
	GameObject * o = create_game_object(type,guid);
	switch(type)
	{
	case EGOT_THROWABLE_OBJECT:
		{
			m_tmp_object_list.push_back(o);
			break;
		}
	default:
			m_game_object_list.push_back(o);
			break;
	}
	
	if(!m_on_add_game_object.is_null())
		m_on_add_game_object.invoke(o);

	return o;
}

void GameObjectManager::remove_game_object(uint32_t guid)
{
	auto it = std::find_if(m_game_object_list.begin(), m_game_object_list.end(), [&guid](GameObject * o){return o->get_guid()==guid;});

	if(it==m_game_object_list.end())
	{	
		it = std::find_if(m_tmp_object_list.begin(), m_tmp_object_list.end(), [&guid](GameObject * o){return o->get_guid()==guid;});
		
		if(it==m_tmp_object_list.end())
			return;

		if(!m_on_remove_game_object.is_null())
			m_on_remove_game_object.invoke(*it);

		m_tmp_object_list.erase(it);
	}
	else
	{
		if(!m_on_remove_game_object.is_null())
			m_on_remove_game_object.invoke(*it);

		m_game_object_list.erase(it);
	}
}

void GameObjectManager::remove_not_alive_objects()
{
	for(auto it = m_game_object_list.begin(); it!=m_game_object_list.end();)
	{
		if(!(*it)->is_alive())
		{
			if(!m_on_remove_game_object.is_null())
				m_on_remove_game_object.invoke(*it);

			it = m_game_object_list.erase(it);
		}
		else
			it++;
	}

	for(auto it = m_tmp_object_list.begin(); it!=m_tmp_object_list.end();)
	{
		if(!(*it)->is_alive())
		{
			if(!m_on_remove_game_object.is_null())
				m_on_remove_game_object.invoke(*it);

			it = m_tmp_object_list.erase(it);
		}
		else
			it++;
	}
}

GameObject * GameObjectManager::find_game_object_by_id(uint32_t id)
{
	auto it = std::find_if(m_game_object_list.begin(), m_game_object_list.end(), [&id](GameObject * o){return o->get_id()==id;});

	if(it==m_game_object_list.end())
	{	
		it = std::find_if(m_tmp_object_list.begin(), m_tmp_object_list.end(), [&id](GameObject * o){return o->get_id()==id;});
		
		if(it==m_tmp_object_list.end())
			return nullptr;

		return (*it);
	}
	else
		return (*it);
}


GameObject * GameObjectManager::find_game_object_by_guid(uint32_t guid)
{
	auto it = std::find_if(m_game_object_list.begin(), m_game_object_list.end(), [&guid](GameObject * o){return o->get_guid()==guid;});

	if(it==m_game_object_list.end())
	{
		it = std::find_if(m_tmp_object_list.begin(), m_tmp_object_list.end(), [&guid](GameObject * o){return o->get_guid()==guid;});
			
		if(it==m_tmp_object_list.end())
			return nullptr;
	}
	
	return (*it);
}


void GameObjectManager::update_game_objects(const clan::GameTime & game_time)
{
	for(auto it = m_game_object_list.begin(); it!=m_game_object_list.end(); it++)
	{
		(*it)->update(game_time);

		if(!m_on_update_game_object.is_null())
			m_on_update_game_object.invoke(*it);

	}

	for(auto itg = m_tmp_object_list.begin(); itg!=m_tmp_object_list.end(); itg++)
	{
		(*itg)->update(game_time);

		if(!m_on_update_game_object.is_null())
			m_on_update_game_object.invoke(*itg);
	}
}


void GameObjectManager::collide_game_objects(const clan::GameTime & game_time)
{
	for(uint32_t i = 0; i<m_game_object_list.size(); i++)
	for(uint32_t j = 0; j<m_tmp_object_list.size(); j++)
	{
		clan::CollisionOutline & outline = m_game_object_list[i]->get_outline();
		clan::CollisionOutline & outline2 = m_tmp_object_list[j]->get_outline();

		outline.enable_collision_info(true, true, false);
		outline2.enable_collision_info(true, true, false);

		if( outline.collide(outline2) )
		{
			clan::log_event("game_ev","Game objects colliding: %1<->%2",m_game_object_list[i]->get_guid(),m_tmp_object_list[j]->get_guid());
			m_game_object_list[i]->on_collide(m_tmp_object_list[j]);

			const std::vector<clan::CollidingContours> &colpointinfo = outline.get_collision_info();
			// Loop through all pairs of colliding contours
			for(size_t c = 0; c < colpointinfo.size(); c++)
			{
					const clan::CollidingContours &cc = colpointinfo[c];
					for(size_t p = 0; p < cc.points.size(); p++)
					{
							//std::cout << "Collision: Point(" << cc.points[p].point.x << "," << cc.points[p].point.y << ")\n";
							//std::cout << "Collision: Normal(" << cc.points[p].normal.x << "," << cc.points[p].normal.y << ")\n";
					}
			}
		}
	}

}


void GameObjectManager::render_game_objects(clan::Canvas & canvas, const clan::vec2 & offset)
{
	for(auto it = m_game_object_list.begin(); it!=m_game_object_list.end(); it++)
		(*it)->render(canvas,offset);

	for(auto it = m_tmp_object_list.begin(); it!=m_tmp_object_list.end(); it++)
		(*it)->render(canvas,offset);
}

clan::Callback_v1<GameObject*> & GameObjectManager::func_on_update_game_object()
{
	return m_on_update_game_object;
}

clan::Callback_v1<GameObject*> & GameObjectManager::func_on_remove_game_object()
{
	return m_on_remove_game_object;
}

clan::Callback_v1<GameObject*> & GameObjectManager::func_on_add_game_object()
{
	return m_on_add_game_object;
}

void GameObjectManager::on_net_event(const clan::NetGameEvent & e)
{
	uint32_t type = e.get_argument(0).to_uinteger();
	clan::log_event("net_event","Got message from server type='%1' msg='%2'.",type,e.to_string());

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