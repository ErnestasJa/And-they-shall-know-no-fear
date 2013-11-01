#pragma once

class SpriteFrameSelection: public clan::GUIComponent
{
protected:
	clan::Sprite	m_sprite;
	clan::vec2		m_pos,m_pan;

	int32_t			m_selected_frame;
	clan::Signal_v1<int32_t> m_sig;

public:
	SpriteFrameSelection(clan::Window * root);
	virtual ~SpriteFrameSelection();
	clan::Signal_v1<int32_t> & func_frame_selected();
	void set_sprite(clan::Sprite & s);
	void render(clan::Canvas & c, const clan::Rect & clip_rect);
	void on_message(std::shared_ptr<clan::GUIMessage> &msg);
	void clamp_pos();
	void edge_pan(const clan::vec2 & pos);
};