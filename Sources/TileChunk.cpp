#include "precomp.h"

#include "TileChunk.h"

class TileChunk_Impl
{
    public:
        TileChunk_Impl(TileMap & tmap, uint32_t id){m_tile_map = tmap; m_id = id;}
        virtual ~TileChunk_Impl(){};

        /**< argument pos should be relative to screen */
        void draw_chunk(Canvas & canvas, const Vec2<int32_t> & pos)
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
        uint32_t     m_id;
        TileLayer    m_layer;
        TileMap m_tile_map;
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
