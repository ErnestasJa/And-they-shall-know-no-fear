#include "precomp.h"
#include "sprite_selection.h"

SpriteSelection::SpriteSelection(clan::Window * root):clan::GUIComponent(root)
{
	set_constant_repaint(true);

	func_render().set(this, &SpriteSelection::render);
	func_process_message().set(this, &SpriteSelection::on_message);
	this->set_geometry(root->get_client_area());
}

SpriteSelection::~SpriteSelection()
{
		
}

clan::Signal_v1<int32_t> & SpriteSelection::func_frame_selected()
{
	return m_sig;
}

void SpriteSelection::set_sprite(clan::Sprite & s)
{
	m_sprite = s.clone();
	m_pos=clan::vec2(0,0);
	m_selected_frame=-1;
}

void SpriteSelection::render(clan::Canvas & c, const clan::Rect & clip_rect)
{
	m_pos+=m_pan;
	clamp_pos();

	int32_t w=this->get_content_box().get_width(),h=this->get_content_box().get_height();

	this->push_cliprect(c,this->get_content_box());

	for(int i = 0; i < 32; i++)
	for(int j = 0; j < 32;  j++)
	{
		m_sprite.set_frame((i*32)+j);
		m_sprite.draw(c,j*32 - m_pos.x,i*32 - m_pos.y);
	}

	this->pop_cliprect(c);
}

void SpriteSelection::on_message(std::shared_ptr<clan::GUIMessage> &msg)
{
	std::shared_ptr<clan::GUIMessage_Input> input_msg = std::dynamic_pointer_cast<clan::GUIMessage_Input>(msg);
	if (input_msg)
	{
		clan::InputEvent &e = input_msg->input_event;
		if (e.type == clan::InputEvent::pointer_moved)
		{
			edge_pan(e.mouse_pos);
		}

		if (e.device.get_type() == clan::InputDevice::pointer && e.type == clan::InputEvent::released && e.id == clan::mouse_left )
		{
			clan::vec2 ats = m_pos + e.mouse_pos;
			ats.x/=32;
			ats.y/=32;

			m_selected_frame = ats.y*32 + ats.x;
			m_sig.invoke(m_selected_frame);
		}
	}
}

void SpriteSelection::clamp_pos()
{
	int32_t w=this->get_content_box().get_width(),h=this->get_content_box().get_height();

	if(m_pos.x<0) m_pos.x = 0;
	if(m_pos.y<0) m_pos.y = 0;

	if(m_pos.x>1024-w) m_pos.x = 1024-w;
	if(m_pos.y>1024-h) m_pos.y = 1024-h;
}

void SpriteSelection::edge_pan(const clan::vec2 & pos)
{
	int32_t sens=15;

	if (pos.x < 30) m_pan.x=-sens;
	else if (pos.x > this->get_content_box().get_width()-10) m_pan.x=sens;
	else m_pan.x=0;

	if (pos.y < 30) m_pan.y=-sens;
	else if (pos.y > this->get_content_box().get_height()-10) m_pan.y=sens;
	else m_pan.y=0;
}