#include "precomp.h"
#include "game_objects.h"
#include "game_object_manager.h"
#include "../net/message.h"

#if defined GAME_SERVER
#include "tile_chunk.h"
#endif

GameObjectManager::GameObjectManager(TileMap map)
{
	m_tile_map = map;
	register_game_object<Player>();
	register_game_object<ThrowableObject>();

	#if defined GAME_SERVER
	clan::Contour contour;
	contour.get_points().push_back(clan::Pointf(0,0));
	contour.get_points().push_back(clan::Pointf(0,32));
	contour.get_points().push_back(clan::Pointf(32,32));
	contour.get_points().push_back(clan::Pointf(32,0));

	m_tile_outline.get_contours().push_back(contour);
	m_tile_outline.calculate_radius();
	m_tile_outline.calculate_smallest_enclosing_discs();
	m_tile_outline.enable_collision_info(true, true, false);
	#endif
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
#if defined GAME_SERVER
	for(uint32_t i = 0; i<m_game_object_list.size(); i++)
	{
		for(uint32_t j = 0; j<m_tmp_object_list.size(); j++)
		{
			clan::CollisionOutline & outline = m_game_object_list[i]->get_outline();
			clan::CollisionOutline & outline2 = m_tmp_object_list[j]->get_outline();

			outline.enable_collision_info(true, true, false);
			outline2.enable_collision_info(true, true, false);

			if(outline.collide(outline2))
			{
				m_game_object_list[i]->on_collide(m_tmp_object_list[j]);
			}
		}


		clan::vec2 tl, br, tl2, br2, pos;
		pos = m_game_object_list[i]->get_pos().get();
		uint32_t w = m_game_object_list[i]->get_outline().get_width(), h = m_game_object_list[i]->get_outline().get_height();
		tl = pos+clan::vec2(16,13);
		br = pos+clan::vec2(31,50);

		tl=pixel_to_chunk_pos(tl);
		br=pixel_to_chunk_pos(br);

        TileChunk c;

		for(int y = tl.y; y <= br.y; y++)
		for(int x = tl.x; x <= br.x; x++)
		{
			c = m_tile_map.get_chunk(clan::vec2(x,y));

			if(!c.is_null())
			{
				tl2=pos+clan::vec2(16,13); ///FIX ME: skaiciai paimti is player constructor
				br2=pos+clan::vec2(47,63);

				tl2=pixel_to_tile_pos(tl2);
				br2=pixel_to_tile_pos(br2);

				for(int j = tl2.y; j <= br2.y; j++)
				for(int k = tl2.x; k <= br2.x; k++)
				{
					Tile t = c.get_tile(clan::vec2(k,j),GROUND_LAYER_COUNT);
					clan::vec2 tile_pos = chunk_to_pixel_pos(clan::vec2(x,y))+clan::vec2(k*32,j*32);

					if(t.type != ETT_NO_TILE)
					{
						m_tile_outline.set_translation(tile_pos.x,tile_pos.y);
						if(m_game_object_list[i]->get_outline().collide(m_tile_outline))
						{
							m_game_object_list[i]->on_tile_collide(t);
						}
					}
				}
			}
		}

		

	}
#endif
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