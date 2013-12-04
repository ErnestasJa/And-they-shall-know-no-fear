#ifndef SPRITE_FRAME_SELECTION_H
#define SPRITE_FRAME_SELECTION_H

class SpriteFrameSelection: public clan::GUIComponent
{
protected:
	clan::Sprite	m_sprite;
	clan::GameTime  m_game_time;
	clan::vec2		m_pos, ats, m_scroll, m_drag_offset;

	int32_t			m_selected_frame;
	clan::Signal_v1<int32_t> m_sig;

public:
	SpriteFrameSelection(clan::Window * root, clan::GameTime & time);
	virtual ~SpriteFrameSelection();
	clan::Signal_v1<int32_t> & func_frame_selected();
	void set_sprite(clan::Sprite s);
	void render(clan::Canvas & c, const clan::Rect & clip_rect);
	void on_message(std::shared_ptr<clan::GUIMessage> &msg);
	void clamp_pos();
};

#endif // SPRITE_FRAME_SELECTION_H
