#include "precomp.h"

#include "TileChunk.h"

class TileChunk_Impl
{
    public:
        TileChunk_Impl(TileMap & tmap, uint32_t id){
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
			Texture2D m_tex = Texture2D(c,TILE_SIZE*TILE_COUNT,TILE_SIZE*TILE_COUNT);

			if(m_tex.is_null())
				throw Exception("texture is null");

			m_fb = FrameBuffer(c);
			m_fb.attach_color(0,m_tex);


			Canvas c2(c,m_fb);
			draw_for_batch(c2,Vec2<int32_t>(0,0));
			c2.flush();
			m_fb.detach_color(0);
			

			m_image = Image(m_tex,Rect(0,0,TILE_SIZE*TILE_COUNT,TILE_SIZE*TILE_COUNT));
			m_batched = true;
		}

        /**< argument pos should be relative to screen */
        void draw_for_batch(Canvas & canvas, const Vec2<int32_t> & pos)
        {
            Sprite s;
            int32_t sid = -1;
            for(int32_t i = 0; i < TILE_COUNT; i++)
            for(int32_t j = 0; j < TILE_COUNT; j++)
            {
                const Tile & t = m_layer.tile[i*TILE_COUNT + j];

                if(sid!=t.sprite_ID)
                {
                    sid = t.sprite_ID;
                    s = m_tile_map.get_sprite(t.sprite_ID);
                }

                s.set_frame(t.sprite_frame);
                s.draw(canvas,pos.x+j*TILE_SIZE,pos.y+i*TILE_SIZE);
            }
        }

		void draw_chunk(Canvas & canvas, const Vec2<int32_t> & pos)
        {
			m_image.draw(canvas,pos.x,pos.y);
        }

        uint8_t get_tile_type(const Vec2<int32_t> & pos)
        {
            if(pos.x>=TILE_COUNT || pos.y>=TILE_COUNT)
            {
                return ETT_NO_TILE;
            }

            return m_layer.tile[pos.y*TILE_COUNT + pos.x].type;
        }

        uint32_t get_id()
        {
            return m_id;
        }

    protected:
        uint32_t    m_id;
        TileLayer   m_layer;
        TileMap		m_tile_map;
		
		///---------
		Image		m_image;
		FrameBuffer m_fb;
		bool m_batched;
};

TileChunk::TileChunk(){}
TileChunk::TileChunk(TileMap & tmap, uint32_t id){impl=std::shared_ptr<TileChunk_Impl>(new TileChunk_Impl(tmap,id));}

void TileChunk::draw_chunk(Canvas & canvas, const Vec2<int32_t> & pos)
{
    impl->draw_chunk(canvas,pos);
}

uint8_t TileChunk::get_tile_type(const Vec2<int32_t> & pos)
{
    return impl->get_tile_type(pos);
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