#ifndef TILEMAP_H
#define TILEMAP_H

struct Tile;
class TileChunk;
class TileMap_Impl;
class TileMap
{
    public:

    TileMap();
    TileMap(Canvas & c, Tile default_tile);
    virtual ~TileMap();


    bool is_null() const { return !impl; }
    void throw_if_null() { if (!impl) throw Exception("TileMap is null"); }


    bool add_sprite(Sprite spr, uint8_t id);
    Sprite get_sprite(uint8_t id);
    void remove_sprite(uint8_t id);


    bool is_chunk_visible(const Vec2<int32_t> & chunk_pos, const Rect & render_rect);
    TileChunk add_chunk( const Vec2<int32_t> & pos );
    TileChunk get_chunk(const Vec2<int32_t> & pos);

    Tile get_default_tile();
	Canvas & get_canvas();


    void render(const Vec2<int32_t> & pos);

    protected:
    std::shared_ptr<TileMap_Impl> impl;
};

#endif // TILEMAP_H
