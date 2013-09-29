#include "precomp.h"

#include "TileChunk.h"

class TileChunk_Impl
{
    public:
        TileChunk_Impl(TileMap & tmap, uint32_t id)
		{
			m_tile_map = tmap; 
			m_id = id;
			m_batched = false;
		}

        virtual ~TileChunk_Impl(){};

		bool is_batched()
		{
			return m_batched;
		}

		void set_batched(bool is_batched)
		{
			m_batched = is_batched;
		}

		void batch()
		{
			Canvas & c = m_tile_map.get_canvas();
			for(int32_t i = 0; i < LAYER_COUNT; i++)
			{
				Texture2D m_tex = Texture2D(c,TILE_SIZE*TILE_COUNT,TILE_SIZE*TILE_COUNT);

				if(m_tex.is_null())
					throw Exception("texture is null");

				m_fb = FrameBuffer(c);
				m_fb.attach_color(0,m_tex);


				Canvas c2(c,m_fb);
				draw_for_batch(c2,i);
				c2.flush();
				m_fb.detach_color(0);
			

				m_layers[i].batched_layer = Image(m_tex,Rect(0,0,TILE_SIZE*TILE_COUNT,TILE_SIZE*TILE_COUNT));
			}
			m_batched = true;
		}

        void draw_for_batch(Canvas & canvas, int32_t layer)
        {
            Sprite s;
            int32_t sid = -1;

            for(int32_t i = 0; i < TILE_COUNT; i++)
            for(int32_t j = 0; j < TILE_COUNT; j++)
            {
                const Tile & t = m_layers[layer].tile[i*TILE_COUNT + j];

				if(t.type==ETT_NO_TILE)
					continue;

                if(sid!=t.sprite_ID)
                {
                    sid = t.sprite_ID;
                    s = m_tile_map.get_sprite(t.sprite_ID);
                }

                s.set_frame(t.sprite_frame);
                s.draw(canvas,j*TILE_SIZE,i*TILE_SIZE);
            }
        }

		void draw_chunk(Canvas & canvas, const vec2 & pos, int32_t layer)
        {
			m_layers[layer].batched_layer.draw(canvas,pos.x,pos.y);
        }

        Tile & get_tile(const vec2 & pos, int32_t layer)
        {
			return m_layers[layer].tile[pos.y*TILE_COUNT + pos.x];
        }

        uint32_t get_id()
        {
            return m_id;
        }

    protected:
        uint32_t    m_id;
        TileLayer   m_layers[LAYER_COUNT];
        TileMap		m_tile_map;
		
		///---------
		FrameBuffer m_fb;
		bool m_batched;
};

TileChunk::TileChunk(){}
TileChunk::TileChunk(TileMap & tmap, uint32_t id){impl=std::shared_ptr<TileChunk_Impl>(new TileChunk_Impl(tmap,id));}

void TileChunk::draw_chunk(Canvas & canvas, const vec2 & pos, int32_t layer)
{
    impl->draw_chunk(canvas,pos,layer);
}

Tile & TileChunk::get_tile(const vec2 & pos, int32_t layer)
{
    return impl->get_tile(pos,layer);
}

uint32_t TileChunk::get_id()
{
    return impl->get_id();
}

bool TileChunk::is_batched()
{
	return impl->is_batched();
}

void TileChunk::set_batched(bool is_batched)
{
	impl->set_batched(is_batched);
}

void TileChunk::batch()
{
	impl->batch();
}