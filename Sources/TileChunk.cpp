#include "precomp.h"

#include "TileChunk.h"

class TileChunk_Impl
{
    public:
        TileChunk_Impl(TileMap & tmap)
		{
			m_tile_map = tmap; 
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
			clan::Canvas & c = m_tile_map.get_canvas();
			for(int32_t i = 0; i < LAYER_COUNT; i++)
			{
				clan::Texture2D m_tex = clan::Texture2D(c,TILE_SIZE*TILE_COUNT,TILE_SIZE*TILE_COUNT);

				if(m_tex.is_null())
					throw clan::Exception("texture is null");

				m_fb = clan::FrameBuffer(c);
				m_fb.attach_color(0,m_tex);


				clan::Canvas c2(c,m_fb);
				draw_for_batch(c2,i);
				c2.flush();
				m_fb.detach_color(0);
			

				m_layers[i].batched_layer = clan::Image(m_tex,clan::Rect(0,0,TILE_SIZE*TILE_COUNT,TILE_SIZE*TILE_COUNT));
			}
			m_batched = true;
		}

        void draw_for_batch(clan::Canvas & canvas, int32_t layer, const clan::vec2 & pos=clan::vec2(0,0))
        {
            clan::Sprite s;
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
                s.draw(canvas,pos.x+(j*TILE_SIZE),pos.y+(i*TILE_SIZE));
            }
        }

		void draw_chunk(clan::Canvas & canvas, const clan::vec2 & pos, int32_t layer, bool draw_batched=true)
        {
			if(draw_batched)
				m_layers[layer].batched_layer.draw(canvas,pos.x,pos.y);
			else
				draw_for_batch(canvas,layer,pos);
        }

        Tile & get_tile(const clan::vec2 & pos, int32_t layer)
        {
			return m_layers[layer].tile[pos.y*TILE_COUNT + pos.x];
        }

		TileLayer & get_tile_layer(int32_t layer)
		{
			return m_layers[layer];
		}

    protected:
        TileLayer   m_layers[LAYER_COUNT];
        TileMap		m_tile_map;
		
		///---------
		clan::FrameBuffer m_fb;
		bool m_batched;
};

TileChunk::TileChunk(){}
TileChunk::TileChunk(TileMap & tmap){impl=std::shared_ptr<TileChunk_Impl>(new TileChunk_Impl(tmap));}

void TileChunk::draw_chunk(clan::Canvas & canvas, const clan::vec2 & pos, int32_t layer, bool draw_batched)
{
    impl->draw_chunk(canvas,pos,layer,draw_batched);
}

Tile & TileChunk::get_tile(const clan::vec2 & pos, int32_t layer)
{
    return impl->get_tile(pos,layer);
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

TileLayer & TileChunk::get_tile_layer(int32_t layer)
{
	return impl->get_tile_layer(layer);
}