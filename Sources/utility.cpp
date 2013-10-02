#include "precomp.h"
#include "utility.h"

clan::vec2 pixel_to_chunk_pos (const clan::vec2 & world_pos_in_pixels)
{
	clan::vec2 ats=world_pos_in_pixels;

	if(ats.x<0)
		ats.x-=CHUNK_EDGE_LENGTH_PIXELS;

	if(ats.y<0)
		ats.y-=CHUNK_EDGE_LENGTH_PIXELS;

	ats.x /= CHUNK_EDGE_LENGTH_PIXELS;
	ats.y /= CHUNK_EDGE_LENGTH_PIXELS;

	return ats;
}