#ifndef TILECHUNK_H
#define TILECHUNK_H

#include "tile_map.h"

enum TILE_TYPE
{
	ETT_INVALID = 0,
    ETT_NO_TILE,
    ETT_NORMAL,
    ETT_WALL,
};

struct Tile
{
    uint8_t type;
    uint8_t sprite_ID;
    uint16_t sprite_frame;

    Tile()
    {
        type=ETT_NO_TILE;
        sprite_ID=0;
        sprite_frame=0;
    }
};

struct TileLayer
{
    Tile tile[TILE_COUNT_IN_CHUNK];
	clan::Image batched_layer;
};

/**< add physical layer for chunk */
class TileChunk_Impl;
class TileChunk
{
    public:
        /**< create null instance */
        TileChunk();
        TileChunk(TileMap & tmap);

        virtual ~TileChunk(){}

        bool is_null() const { return !impl; }
        void throw_if_null() { if (!impl) throw clan::Exception("TileChunk is null"); }

        void draw_chunk(clan::Canvas & canvas, const clan::vec2 & pos, int32_t layer, bool draw_batched=true);

		bool is_batched();
		void set_batched(bool is_batched); // to force update
		void batch();

        Tile & get_tile(const clan::vec2 & pos, int32_t layer);
		TileLayer & get_tile_layer(int32_t layer);
    protected:
        std::shared_ptr<TileChunk_Impl> impl;
};

#endif // TILECHUNK_H
