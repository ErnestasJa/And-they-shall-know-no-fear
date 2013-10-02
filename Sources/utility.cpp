#include "precomp.h"
#include "utility.h"

clan::vec2 pixel_to_cunk_pos (const clan::vec2 & pos)
{
	clan::vec2 ats;

	ats.x = (pos.x / CHUNK_EDGE_LENGTH_PIXELS)*CHUNK_EDGE_LENGTH_PIXELS;
	ats.y = (pos.y / CHUNK_EDGE_LENGTH_PIXELS)*CHUNK_EDGE_LENGTH_PIXELS;

	return ats;
}