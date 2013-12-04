#include "precomp.h"
#include "sprite_frame_selection.h"

SpriteFrameSelection::SpriteFrameSelection(clan::Window * root, clan::GameTime & time):clan::GUIComponent(root)
{
	set_constant_repaint(true);
	m_game_time = time;

	func_render().set(this, &SpriteFrameSelection::render);
	func_process_message().set(this, &SpriteFrameSelection::on_message);
	this->set_geometry(root->get_client_area());
}

SpriteFrameSelection::~SpriteFrameSelection()
{

}

clan::Signal_v1<int32_t> & SpriteFrameSelection::func_frame_selected()
{
	return m_sig;
}

void SpriteFrameSelection::set_sprite(clan::Sprite s)
{
	m_sprite = s.clone();
	m_pos=clan::vec2(0,0);
	m_selected_frame=-1;
}

void SpriteFrameSelection::render(clan::Canvas & c, const clan::Rect & clip_rect)
{
	m_pos+=m_scroll;
	clamp_pos();
	int32_t w=this->get_content_box().get_width(),h=this->get_content_box().get_height();
	this->push_cliprect(c,this->get_content_box());

	for(int i = 0; i < 32; i++)
	for(int j = 0; j < 32; j++)
	{
		m_sprite.set_frame((i*32)+j);
		m_sprite.draw(c,j*32 - m_pos.x,i*32 - m_pos.y);
	}

	for(int32_t i=0; i<=w; i++)if((i+m_pos.x)%TILE_SIZE==0 ) c.draw_line(clan::LineSegment2f(clan::vec2(i,0),clan::vec2(i,h)),clan::Colorf::deeppink);
	for(int32_t i=0; i<=h; i++)if((i+m_pos.y)%TILE_SIZE==0 ) c.draw_line(clan::LineSegment2f(clan::vec2(0,i),clan::vec2(w,i)),clan::Colorf::deeppink);
	this->pop_cliprect(c);

	
}

void SpriteFrameSelection::on_message(std::shared_ptr<clan::GUIMessage> &msg)
{
	std::shared_ptr<clan::GUIMessage_Input> input_msg = std::dynamic_pointer_cast<clan::GUIMessage_Input>(msg);
	if (input_msg)
	{
		clan::InputEvent &e = input_msg->input_event;

		if(e.device.get_type() == clan::InputDevice::pointer)
		{
			if (e.type == clan::InputEvent::pressed && e.id == clan::mouse_middle)
			{
				m_drag_offset = e.mouse_pos;
			}
			else if (e.type == clan::InputEvent::released && e.id == clan::mouse_middle)
			{
				m_scroll=clan::vec2();
			}
			else if (e.type == clan::InputEvent::pointer_moved)
			{
				if (e.device.get_keycode(clan::mouse_middle))
					m_scroll=(e.mouse_pos-m_drag_offset)/m_game_time.get_time_elapsed_ms();
			}

			if ( e.type == clan::InputEvent::released && e.id == clan::mouse_left )
			{
				clan::vec2 ats = m_pos + e.mouse_pos;
				ats.x/=32;
				ats.y/=32;

				m_selected_frame = ats.y*32 + ats.x;
				m_sig.invoke(m_selected_frame);
			}

		}
	}
}

void SpriteFrameSelection::clamp_pos()
{
	int32_t w=this->get_content_box().get_width(),h=this->get_content_box().get_height();

	if(m_pos.x<0) m_pos.x = 0;
	if(m_pos.y<0) m_pos.y = 0;

	if(m_pos.x>1024-w) m_pos.x = 1024-w;
	if(m_pos.y>1024-h) m_pos.y = 1024-h;
}
