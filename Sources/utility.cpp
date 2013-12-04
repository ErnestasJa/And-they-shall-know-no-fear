#include "precomp.h"
#include "utility.h"

clan::vec2 pixel_to_chunk_pos (const clan::vec2 & pos)
{
	clan::vec2 ats;

	ats.x = pos.x / CHUNK_EDGE_LENGTH_PIXELS; if(pos.x<0) ats.x--;
	ats.y = pos.y / CHUNK_EDGE_LENGTH_PIXELS; if(pos.y<0) ats.y--;

	return ats;
}


clan::vec2 pixel_to_tile_pos (const clan::vec2 & pos)
{
	clan::vec2 ats;

	ats.x = (abs(pos.x) % CHUNK_EDGE_LENGTH_PIXELS) / TILE_SIZE; if(pos.x<0) ats.x=TILE_COUNT-1-ats.x;
	ats.y = (abs(pos.y) % CHUNK_EDGE_LENGTH_PIXELS) / TILE_SIZE; if(pos.y<0) ats.y=TILE_COUNT-1-ats.y;

	return ats;
}