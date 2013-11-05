#pragma once

#define BIT(x) 1<<(x)
#define CHECK_BIT(val,bit) (((val) & (bit)) == (bit))
clan::vec2 pixel_to_chunk_pos (const clan::vec2 & pos);
clan::vec2 pixel_to_tile_pos (const clan::vec2 & pos);

