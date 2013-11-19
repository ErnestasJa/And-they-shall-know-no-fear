#ifndef TILEMAP_H
#define TILEMAP_H

struct Tile;
class TileChunk;
class TileMap_Impl;
class TileMap
{
    public:

    TileMap();
    TileMap(clan::Canvas & c);
    virtual ~TileMap();

    bool is_null() const { return !impl; }
    void throw_if_null() { if (!impl) throw clan::Exception("TileMap is null"); }


    bool			add_sprite(clan::Sprite spr, uint8_t id);
    clan::Sprite	get_sprite(uint8_t id);
    void			remove_sprite(uint8_t id);


    bool		is_chunk_visible(const clan::vec2 & chunk_pos, const clan::Rect & render_rect);
    TileChunk	add_chunk( const clan::vec2 & pos );
    TileChunk	get_chunk( const clan::vec2 & pos );
	void		erase_chunk( const clan::vec2 & pos );

	clan::Canvas &	get_canvas();
    void			render(const clan::vec2 & pos);
	
	bool load(const std::string & file);
	bool save(const std::string & file);

    protected:
    std::shared_ptr<TileMap_Impl> impl;
};

#endif // TILEMAP_H
