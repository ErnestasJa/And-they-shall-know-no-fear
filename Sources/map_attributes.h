#ifndef MAP_ATTRIBUTES_H
#define MAP_ATTRIBUTES_H


static const int32_t VERSION = 1;

static const int32_t TILE_SIZE = 32; /// tile dydis pixeliais (32x32)
static const int32_t TILE_COUNT = 16; /// tile kiekis vienoje bloko eileje
static const int32_t TILE_COUNT_IN_CHUNK = TILE_COUNT * TILE_COUNT;
static const int32_t CHUNK_EDGE_LENGTH_PIXELS = TILE_SIZE * TILE_COUNT;

static const int32_t GROUND_LAYER_COUNT = 2;
static const int32_t OBJECT_LAYER_COUNT = 1;
static const int32_t LAYER_COUNT = GROUND_LAYER_COUNT+OBJECT_LAYER_COUNT;

#endif // MAP_ATTRIBUTES_H
