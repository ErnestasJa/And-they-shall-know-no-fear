#ifndef TILEMAP_H
#define TILEMAP_H

struct Tile;
struct spawn
{
	uint32_t object_type;
	clan::vec2 pos;
};

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

	bool			add_sprite(std::string resource_id, uint8_t id);
    clan::Sprite	get_sprite(uint8_t id);
	clan::Sprite	get_sprite(std::string resource_id);
    void			remove_sprite(uint8_t id);

	void load_resource_document(const std::string & file_name);
	clan::XMLResourceDocument get_resource_document();

    bool		is_chunk_visible(const clan::vec2 & chunk_pos, const clan::Rect & render_rect);
    TileChunk	add_chunk( const clan::vec2 & pos );
    TileChunk	get_chunk( const clan::vec2 & pos );
	void		erase_chunk( const clan::vec2 & pos );

	clan::Canvas &	get_canvas();
	void		render(const clan::vec2 & pos, int start_layer=0, int end_layer=LAYER_COUNT);
	void        render_spawns(const clan::vec2 & pos);

	void		add_spawn(uint32_t object_type, clan::vec2 pos);
	void		delete_spawn(clan::vec2 pos, int32_t range = 50);

	bool		load(const std::string & file);
	bool		save(const std::string & file);

    protected:
    std::shared_ptr<TileMap_Impl> impl;
	std::vector<spawn> m_spawn_list;
};

#endif // TILEMAP_H
