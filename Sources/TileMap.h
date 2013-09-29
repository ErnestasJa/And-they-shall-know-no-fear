#ifndef TILEMAP_H
#define TILEMAP_H

struct Tile;
class TileChunk;
class TileMap_Impl;
class TileMap
{
    public:

    TileMap();
    TileMap(Canvas & c);
    virtual ~TileMap();


    bool is_null() const { return !impl; }
    void throw_if_null() { if (!impl) throw Exception("TileMap is null"); }

    bool add_sprite(Sprite spr, uint8_t id);
    Sprite get_sprite(uint8_t id);
    void remove_sprite(uint8_t id);

    bool is_chunk_visible(const vec2 & chunk_pos, const Rect & render_rect);
    TileChunk add_chunk( const vec2 & pos );
    TileChunk get_chunk( const vec2 & pos );

	Canvas & get_canvas();

    void render(const vec2 & pos);

    protected:
    std::shared_ptr<TileMap_Impl> impl;
};

#endif // TILEMAP_H
