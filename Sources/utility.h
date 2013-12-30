#pragma once

#define BIT(x) 1<<(x)

#define loop(V,X) for(uint32_t (V) = 0; (V) < (X); (V)++)
#define loopi(X) loop(i,(X))

clan::vec2 pixel_to_chunk_pos (const clan::vec2 & pos);
clan::vec2 chunk_to_pixel_pos (const clan::vec2 & pos);
clan::vec2 pixel_to_tile_pos (const clan::vec2 & pos);

