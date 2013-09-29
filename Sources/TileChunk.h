#ifndef TILECHUNK_H
#define TILECHUNK_H

#include "TileMap.h"

static const int32_t TILE_SIZE = 32;
static const int32_t  TILE_COUNT = 16;
static const int32_t CHUNK_SIZE = TILE_SIZE * TILE_COUNT;
static const int32_t LAYER_SIZE = CHUNK_SIZE * CHUNK_SIZE;


static const int32_t GROUND_LAYER_COUNT = 2;
static const int32_t OBJECT_LAYER_COUNT = 1;
static const int32_t LAYER_COUNT = GROUND_LAYER_COUNT+OBJECT_LAYER_COUNT;

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
    uint8_t sprite_frame;

    Tile()
    {
        type=ETT_NO_TILE;
        sprite_ID=0;
        sprite_frame=0;
    }
};

struct TileLayer
{
    Tile tile[LAYER_SIZE];
	Image batched_layer;
};

/**< add physical layer for chunk */
class TileChunk_Impl;
class TileChunk
{
    public:
        /**< create null instance */
        TileChunk();
        TileChunk(TileMap & tmap, uint32_t id);

        virtual ~TileChunk(){}

        bool is_null() const { return !impl; }
        void throw_if_null() { if (!impl) throw Exception("TileChunk is null"); }

        void draw_chunk(Canvas & canvas, const vec2 & pos, int32_t layer);

		bool is_batched();
		void set_batched(bool is_batched); // to force update
		void batch();

        Tile & get_tile(const vec2 & pos, int32_t layer);
        uint32_t get_id();

    protected:
        std::shared_ptr<TileChunk_Impl> impl;
};

#endif // TILECHUNK_H
