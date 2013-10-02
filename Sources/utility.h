#pragma once
#include "mapAttributes.h"

clan::vec2 pixel_to_cunk_pos (const clan::vec2 pixel_pos)
{
	clan::vec2 ats;

	ats.x = (pixel_pos.x % CHUNK_EDGE_LENGTH_PIXELS)*CHUNK_EDGE_LENGTH_PIXELS;
	ats.y = (pixel_pos.y % CHUNK_EDGE_LENGTH_PIXELS)*CHUNK_EDGE_LENGTH_PIXELS;

	return ats;
}