#include "precomp.h"

#include "tile_map.h"
#include "tile_chunk.h"


class TileMap_Impl
{

protected:

    clan::Canvas						m_canvas;
    std::map<clan::vec2, TileChunk>		m_chunks;
    //std::map<uint8_t, clan::Sprite>     m_sprites; DEBUG
	std::map<uint8_t, std::pair<std::string, clan::Sprite> >     m_sprites;

public:

    TileMap_Impl(clan::Canvas c)
    {
        m_canvas = c;
    }

	bool add_sprite(clan::Sprite spr, uint8_t id)
    {
        if(m_sprites.find(id)==m_sprites.end())
        {
			m_sprites[id].second=spr;
            return true;
        }
		return false;
    }

    bool add_sprite(clan::Sprite spr, std::string path, uint8_t id)
    {
        if(m_sprites.find(id)==m_sprites.end())
        {
			std::pair<std::string,clan::Sprite> sprite_in_sheet = std::make_pair(path,spr);
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

	clan::Sprite get_sprite(std::string path)
    {
		for (auto i=m_sprites.begin(); i!=m_sprites.end(); ++i)
		{
			if(i->second.first==path)
				return i->second.second;
		}
		throw clan::Exception("get_sprite by path failed to get sprite sheet");
    }

    void remove_sprite(uint8_t id)
    {
        m_sprites.erase(id);
    }

	bool load_resource_document(const std::string & file_name)
	{
		m_doc = clan::XMLResourceDocument(file_name);
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

	bool load(TileMap map, const std::string & file)
	{
		m_chunks.clear();

		try
		{
			clan::File f(file,clan::File::open_existing,clan::File::access_read);

			if(f.is_null())
				return false;

			verify_header(f);

			while(f.get_position()<f.get_size())
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

		write_header(f);

		for(std::map<clan::vec2, TileChunk>::iterator m = m_chunks.begin(); m != m_chunks.end(); m++)
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

bool TileMap::add_sprite(clan::Sprite spr, uint8_t id)
{
    impl->add_sprite(spr,id);

    return false;
}

bool TileMap::add_sprite(clan::Sprite spr, std::string path, uint8_t id)
{
    impl->add_sprite(spr,path,id);

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

bool TileMap::load_resource_document(const std::string & file_name)
{

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
