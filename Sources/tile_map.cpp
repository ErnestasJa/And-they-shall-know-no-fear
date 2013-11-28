#include "precomp.h"

#include "tile_map.h"
#include "tile_chunk.h"

///append enums to end
enum EMapSectionType
{
	EMST_RESOURCE_FILE=1,
	EMST_MAP,
};

struct MS_Resource
{
	std::string res_file;

	void read(clan::File & f)
	{
		res_file	= f.read_string_a();
	}

	void write(clan::File & f)
	{
		f.write_string_a(res_file);
	}
};

struct MapSection
{
	uint32_t type;
	uint32_t data_start;
	uint32_t data_bytes;
	uint32_t next_section_ofs;

	void read(clan::File & f)
	{
		type	= f.read_uint32();
		data_start	= f.read_uint32();
		data_bytes	= f.read_uint32();
		next_section_ofs = f.read_uint32();
	}

	void write(clan::File & f)
	{
		f.write_uint32(type);
		f.write_uint32(data_start);
		f.write_uint32(data_bytes);
		f.write_uint32(next_section_ofs);
	}
};

struct MapHeader
{
	int32_t version;
	int32_t tile_size;

	int32_t tile_count;
	int32_t tile_count_in_chunk;
	int32_t chunk_edge_length_pixels;

	int32_t ground_layer_count;
	int32_t object_layer_count;

	uint32_t section_start;

	void write(clan::File & f)
	{
		f.write_int32(VERSION);

		f.write_int32(TILE_SIZE);
		f.write_int32(TILE_COUNT);
		f.write_int32(TILE_COUNT_IN_CHUNK);
		f.write_int32(CHUNK_EDGE_LENGTH_PIXELS);

		f.write_int32(GROUND_LAYER_COUNT);
		f.write_int32(OBJECT_LAYER_COUNT);
	}

	void read(clan::File & f)
	{
		version = f.read_int32();

		tile_size	= f.read_int32();
		tile_count	= f.read_int32();

		tile_count_in_chunk = f.read_int32();
		chunk_edge_length_pixels = f.read_int32();

		ground_layer_count = f.read_int32();
		object_layer_count = f.read_int32();

		section_start = f.get_position();
	}

	bool check() ///TODO: add nice error output.
	{
		return version == VERSION &&

		tile_size	== TILE_SIZE &&
		tile_count	== TILE_COUNT &&

		tile_count_in_chunk == TILE_COUNT_IN_CHUNK &&
		chunk_edge_length_pixels == CHUNK_EDGE_LENGTH_PIXELS &&

		ground_layer_count == GROUND_LAYER_COUNT &&
		object_layer_count == OBJECT_LAYER_COUNT;
	}
};

class TileMap_Impl
{

protected:

    clan::Canvas						m_canvas;
    std::map<clan::vec2, TileChunk>		m_chunks;
    //std::map<uint8_t, clan::Sprite>     m_sprites; DEBUG
	std::map<uint8_t, std::pair<std::string, clan::Sprite> >     m_sprites;

	std::string					m_doc_file_name;
	clan::XMLResourceDocument	m_doc;

public:

    TileMap_Impl(clan::Canvas c)
    {
        m_canvas = c;
    }

    bool add_sprite(std::string resource_id, uint8_t id)
    {
        if(m_sprites.find(id)==m_sprites.end())
        {
			clan::Sprite spr = clan::Sprite::load(m_canvas,"sprite_sheet_list/"+resource_id,m_doc);
			std::pair<std::string,clan::Sprite> sprite_in_sheet = std::make_pair(resource_id,spr);
            m_sprites[id] = sprite_in_sheet;
            return true;
        }

		return false;
    }

    clan::Sprite get_sprite(uint8_t id)
    {
        auto spr = m_sprites.find(id);
        if(spr!=m_sprites.end())
            return spr->second.second;
		throw clan::Exception("get_sprite by id failed to get sprite sheet");
    }

	clan::Sprite get_sprite(std::string resource_id)
    {
		for (auto i=m_sprites.begin(); i!=m_sprites.end(); ++i)
		{
			if(i->second.first==resource_id)
				return i->second.second;
		}
		throw clan::Exception("get_sprite by path failed to get sprite sheet");
    }

    void remove_sprite(uint8_t id)
    {
        m_sprites.erase(id);
    }

	void load_resource_document(const std::string & file_name)
	{
		m_doc_file_name = file_name;
		m_doc = clan::XMLResourceDocument(m_doc_file_name);

		std::vector<std::string> names = m_doc.get_resource_names("sprite_sheet_list/"); ///gale reikalingas "/"

		uint8_t i = 0;
		for(auto it = names.begin(); it != names.end(); it++)
		{
			std::string name = (*it);
			clan::DomElement el = m_doc.get_resource("sprite_sheet_list/"+name).get_element();

			if(el.get_tag_name()=="sprite")
			{
				if (el.has_attribute("name"))
				{
					std::string ss_name = el.get_attribute("name");
					add_sprite(ss_name, i);
					i++;
				}
			}
		}
	}

	clan::XMLResourceDocument get_resource_document()
	{
		return m_doc;
	}

    bool is_chunk_visible(const clan::vec2 & chunk_pos, const clan::Rect & render_rect)
    {
        clan::Rect chunk_rect(chunk_pos.x,chunk_pos.y, clan::Sizex<int32_t>(CHUNK_EDGE_LENGTH_PIXELS,CHUNK_EDGE_LENGTH_PIXELS));
        return chunk_rect.is_overlapped(render_rect);
    }

    TileChunk add_chunk(TileMap m, const clan::vec2 & pos )
    {
        if(m_chunks.find(pos)==m_chunks.end())
        {
            TileChunk c(m);
            m_chunks[pos]=c;
            return c;
        }
        else
            return TileChunk();
    }


    TileChunk get_chunk(const clan::vec2 & pos)
    {
        auto r = m_chunks.find(pos);
        if(r!=m_chunks.end())
        {
            return r->second;
        }

        return TileChunk();
    }

	void erase_chunk( const clan::vec2 & pos )
	{
		m_chunks.erase(pos);
	}

    clan::Canvas & get_canvas()
    {
        return m_canvas;
    }

    void render(const clan::vec2 & pos)
    {
        int w = m_canvas.get_width();
        int h = m_canvas.get_height();

		clan::vec2 tl, br;
		tl = pos;
		br = pos+clan::vec2(w,h);

		tl=pixel_to_chunk_pos(tl);
		br=pixel_to_chunk_pos(br);

        TileChunk c;

		for(int y = tl.y; y <= br.y; y++)
		for(int x = tl.x; x <= br.x; x++)
		{
			c = get_chunk(clan::vec2(x,y));

			if(!c.is_null())
			{
				if(!c.is_batched())
					c.batch();

				for(int32_t i = 0; i < LAYER_COUNT; i++)
					c.draw_chunk(m_canvas,clan::vec2(x*CHUNK_EDGE_LENGTH_PIXELS,y*CHUNK_EDGE_LENGTH_PIXELS)-pos,i,false);
			}
		}
    }

	void write_map_data(clan::File & f)
	{	
		f.write_uint32(m_chunks.size());

		for(auto m = m_chunks.begin(); m != m_chunks.end(); m++)
		{
			TileChunk c = m->second;

			f.write_int32(m->first.x);
			f.write_int32(m->first.y);

			for(int32_t i = 0; i < LAYER_COUNT; i++)
			{
				TileLayer & l = c.get_tile_layer(i);
				for(int32_t j = 0; j < TILE_COUNT_IN_CHUNK; j++)
				{
					f.write_uint8(l.tile[j].type);
					f.write_uint8(l.tile[j].sprite_ID);
					f.write_uint16(l.tile[j].sprite_frame);
				}
			}
		}
	}

	void read_map_data(TileMap map, clan::File & f)
	{
		uint32_t chunk_count = f.read_uint32();

		for(uint32_t i = 0; i < chunk_count; i++)
		{
			int32_t x = f.read_int32();
			int32_t y = f.read_int32();

			TileChunk c = add_chunk(map, clan::vec2(x,y));

			for(int32_t i = 0; i < LAYER_COUNT; i++)
			{
				TileLayer & l = c.get_tile_layer(i);
				for(int32_t j = 0; j < TILE_COUNT_IN_CHUNK; j++)
				{
					l.tile[j].type=f.read_uint8();
					l.tile[j].sprite_ID=f.read_uint8();
					l.tile[j].sprite_frame=f.read_uint16();
				}
			}
		}
	}

	bool load(TileMap map, const std::string & file)
	{
		m_chunks.clear();

		try
		{
			clan::File f(file,clan::File::open_existing,clan::File::access_read);

			if(f.is_null())
				return false;

			MapHeader h;
			h.read(f);

			if(h.check())
			{
				
				///all is fine, let's do zis
				while(f.get_position()<f.get_size())
				{
					MapSection ms;
					ms.read(f);

					switch(ms.type)
					{
					case EMST_RESOURCE_FILE:
						{
							MS_Resource s;
							s.read(f);
							this->load_resource_document(s.res_file);
							break;
						}
					case EMST_MAP:
						{
							read_map_data(map,f);

							break;
						}
					}
				}
			}
			else
			{
				///all could have been fine, but it wasn't..
				throw clan::Exception("Map could not be loaded.");
			}

			f.close();
		}
		catch(clan::Exception &)
		{
			return false;
		}

		return true;
	}

	bool save(const std::string & file)
	{
		clan::File f(file,clan::File::create_always,clan::File::access_write);

		MapHeader h;
		h.write(f);
		
		uint32_t pos = f.get_position();

		MapSection ms;

		///write chunks
		ms.type = EMST_MAP;
		ms.write(f);
		
		write_map_data(f);

		///write resource file info
		ms.type = EMST_RESOURCE_FILE;
		ms.write(f);

		MS_Resource r;
		r.res_file = m_doc_file_name;
		r.write(f);

		f.close();

		return true;
	}

	protected:

	void write_header(clan::File & f)
	{
		f.write_int32(VERSION);

		f.write_int32(TILE_SIZE);
		f.write_int32(TILE_COUNT);
		f.write_int32(TILE_COUNT_IN_CHUNK);
		f.write_int32(CHUNK_EDGE_LENGTH_PIXELS);

		f.write_int32(GROUND_LAYER_COUNT);
		f.write_int32(OBJECT_LAYER_COUNT);
		f.write_int32(LAYER_COUNT);
	}

	bool verify_header(clan::File & f)
	{
		int32_t version = f.read_int32();

		int32_t tile_size = f.read_int32();
		int32_t tile_count = f.read_int32();
		int32_t tile_count_in_chunk = f.read_int32();
		int32_t chunk_edge_length_pixels = f.read_int32();

		int32_t ground_layer_count = f.read_int32();
		int32_t object_layer_count = f.read_int32();
		int32_t layer_count = f.read_int32();

		return true;
	}
};

TileMap::TileMap()
{

}

TileMap::TileMap(clan::Canvas & c)
{
    impl=std::shared_ptr<TileMap_Impl>(new TileMap_Impl(c));
}

TileMap::~TileMap(){}

bool TileMap::add_sprite(std::string path, uint8_t id)
{
    impl->add_sprite(path,id);

    return false;
}

clan::Sprite TileMap::get_sprite(uint8_t id)
{
    return impl->get_sprite(id);
}

clan::Sprite TileMap::get_sprite(std::string path)
{
    return impl->get_sprite(path);
}

void TileMap::remove_sprite(uint8_t id)
{
    impl->remove_sprite(id);
}

void TileMap::load_resource_document(const std::string & file_name)
{
	impl->load_resource_document(file_name);
}

clan::XMLResourceDocument TileMap::get_resource_document()
{
	return impl->get_resource_document();
}

bool TileMap::is_chunk_visible(const clan::vec2 & chunk_pos, const clan::Rect & render_rect)
{
    return impl->is_chunk_visible(chunk_pos,render_rect);
}

TileChunk TileMap::add_chunk( const clan::vec2 & pos )
{
    return impl->add_chunk(*this,pos);
}


TileChunk TileMap::get_chunk(const clan::vec2 & pos)
{
    return impl->get_chunk(pos);
}

void TileMap::erase_chunk(const clan::vec2 & pos)
{
    return impl->erase_chunk(pos);
}

clan::Canvas & TileMap::get_canvas()
{
	return impl->get_canvas();
}

void TileMap::render(const clan::vec2 & pos)
{
    impl->render(pos);
}

bool TileMap::load(const std::string & file)
{
	return impl->load(*this,file);
}

bool TileMap::save(const std::string & file)
{
	return impl->save(file);
}
