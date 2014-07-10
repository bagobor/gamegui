#include "stdafx.h"

#include "button.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"
#include "eventtypes.h"
#include "utils.h"

#include "../xml/xml.h"

namespace gui
{

Button::Button(System& sys, const std::string& name) :
	Label(sys, name),
	m_state(Normal)
{
}

Button::~Button(void)
{
}

bool Button::onMouseEnter(void)
{
	invalidate();
	m_state = Hovered;
	return Label::onMouseEnter();
}

bool Button::onMouseLeave(void)
{
	invalidate();
	m_state = Normal;
	return Label::onMouseLeave();
}

bool Button::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
{
	if(btn == EventArgs::Left)
	{
		if(state == EventArgs::Down)
		{
			m_system.queryCaptureInput(this);
			m_state = Pushed;
			invalidate();
		}
		else if (m_state == Pushed)
		{
			m_system.queryCaptureInput(0);
			m_state = Hovered;
			
			//point pt = transformToWndCoord(m_system.getCursor().getPosition());
			if(isCursorInside())
			{
				MouseEventArgs m;
				m.name = "On_Clicked";
				callHandler(&m);

				send_event(events::ClickEvent());
			}
			invalidate();
		}
	}
	Label::onMouseButton(btn, state);
	return true;
}

bool Button::onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state)
{
	Label::onKeyboardButton(key, state);

	switch(key)
	{
	case EventArgs::K_SPACE:
	case EventArgs::K_RETURN:
	case EventArgs::K_EXECUTE:
		if(state == EventArgs::Down)
		{
			//m_system.queryCaptureInput(this);
			m_state = Pushed;
		}
		else
		{
			//m_system.queryCaptureInput(0);
			m_state = Normal;
			MouseEventArgs m;
			m.name = "On_Clicked";
			callHandler(&m);

			send_event(events::ClickEvent());
		}
		invalidate();
		return true;
		break;
	};
	return false;
}

bool Button::onCaptureLost(void)
{
	m_state = Normal;
	invalidate();
	return true;
}

void Button::render(const Rect& finalRect, const Rect& finalClip)
{
	States st = m_state;
	/*	if(m_pushed) st = Pushed;
		else if(m_hovered || m_focus) st = Hovered;
		else st = Normal;*/
	StateImagery& state = m_states[st];
	float left = 0;
	float right = 0;
	Rect componentRect;
	Size imgSize;

	Renderer& r = m_system.getRenderer();

	// left image
    if (state.leftImg)
    {
        // calculate final destination area
		imgSize = state.leftImg->size();
        componentRect.m_left = finalRect.m_left;
        componentRect.m_top  = finalRect.m_top;
		componentRect.m_right = finalRect.m_left + imgSize.width;
        componentRect.m_bottom = finalRect.m_bottom;
        componentRect = finalRect.getIntersection (componentRect);

		left  = imgSize.width;

        // draw this element.
		r.draw(*state.leftImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
    }
	// right image
    if (state.rightImg)
    {
		imgSize = state.rightImg->size();
        componentRect.m_left = finalRect.m_right - imgSize.width;
        componentRect.m_top  = finalRect.m_top;
        componentRect.m_right = finalRect.m_right;
		componentRect.m_bottom = finalRect.m_bottom;
        componentRect = finalRect.getIntersection (componentRect);

		right = imgSize.width;

        // draw this element.
		r.draw(*state.rightImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
    }
	// center image
    if (state.backImg)
    {
		componentRect = finalRect;
		componentRect.m_left += left;
		componentRect.m_right -= right;
        
		// draw this element.
		r.draw(*state.backImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Tile, ImageOps::Stretch);
    }

	Label::render(finalRect, finalClip);	
}

void Button::init(xml::node& node)
{
	Label::init(node);

	xml::node frame = node("FrameImagery");
	if(!frame.empty())
	{
		std::string setname = frame["Imageset"].value();
		m_imgset = m_system.getWindowManager().loadImageset(setname);			
		if(m_imgset)
		{
			const Imageset& set = *m_imgset;
			xml::node statenode = frame.first_child();
			while(!statenode.empty())
			{
				States st = getStateByString(statenode["Type"].value());
				StateImagery &s = m_states[st];
				s.backImg = set[statenode("Background")["Image"].value()];
				s.leftImg = set[statenode("Left")["Image"].value()];
				s.rightImg = set[statenode("Right")["Image"].value()];

				statenode = statenode.next_sibling();
			}
		}
	}
}

Button::States Button::getStateByString(const std::string& type)
{
	if(type == "Hovered") return Hovered;
	else if(type == "Pushed") return Pushed;
	else if (type == "Disabled") return Disabled;
	return Normal;
}

ImageButton::ImageButton(System& sys, const std::string& name) :
	Button(sys, name)
{
}

ImageButton::~ImageButton(void)
{
}


void ImageButton::render(const Rect& finalRect, const Rect& finalClip)
{
	States st = m_state;

	//if(m_pushed) 
	//	st = Pushed;
	//else if(m_hovered || m_focus) 
	//	st = Hovered;
	
	const Image* i = m_stateimg[st];
	if(i)
	{	
		Renderer& r = m_system.getRenderer();
		r.draw(*i, finalRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
	}

	Label::render(finalRect, finalClip);
}

void ImageButton::init(xml::node& node)
{
	Label::init(node);

	xml::node frame = node("StateImagery");
	if(!frame.empty())
	{
		std::string setname = frame["Imageset"].value();
		m_imgset = m_system.getWindowManager().loadImageset(setname);
		if(m_imgset)
		{
			const Imageset& set = *m_imgset;
			xml::node statenode = frame.first_child();
			while(!statenode.empty())
			{
				States st = getStateByString(statenode["Type"].value());
				m_stateimg[st] = set[statenode["Image"].value()];

				statenode = statenode.next_sibling();
			}
		}
	}
}

Thumb::Thumb(System& sys, const std::string& name) :
	ImageButton(sys, name),
	m_movable(true),
	m_horiz(true),
	m_offset(0.f, 0.f)
{
}

Thumb::~Thumb(void)
{
}

bool Thumb::onMouseMove(void)
{
	if(m_state == Pushed)
	{
		point pt = transformToWndCoord(m_system.getCursor().getPosition());
		point newpos = pt - m_offset;
		Rect testarea(m_area);
		testarea.setPosition(newpos);

		Size me = m_area.getSize();

		if(testarea.m_left < m_trackarea.m_left)
			testarea.m_left = m_trackarea.m_left;
		if(testarea.m_top < m_trackarea.m_top)
			testarea.m_top = m_trackarea.m_top;
		if(testarea.m_right > m_trackarea.m_right)
			testarea.m_left = m_trackarea.m_right - me.width;
		if(testarea.m_bottom > m_trackarea.m_bottom)
			testarea.m_top = m_trackarea.m_bottom - me.height;

		testarea.setSize(me);
		invalidate();
	
		m_area = testarea;
		send_event(events::TrackEvent());
	}
	return true;
}

bool Thumb::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
{
	if(btn == EventArgs::Left)
	{
		if(state == EventArgs::Down && m_movable)
		{
			m_system.queryCaptureInput(this);
			m_state = Pushed;

			point pt = transformToWndCoord(m_system.getCursor().getPosition());
			m_offset = pt - m_area.getPosition();

		}
		else
		{
			if(m_state == Pushed)
				m_system.queryCaptureInput(0);

			m_state = Hovered;
		}
		invalidate();
	}
	return true;
}

float Thumb::getProgress(void) const
{
	float width = m_trackarea.getWidth();
	float height = m_trackarea.getHeight();

	point pos = m_area.getPosition() - m_trackarea.getPosition();
	Size sz = m_area.getSize();

	if(m_horiz)
	{
		if(width < 1.f || width == sz.width)
			return 0.f;

		return pos.x / (width - sz.width);
	}
	else
	{
		if(height < 1.f || height == sz.height)
			return 0.f;

		return pos.y / (height - sz.height);
	}
}

void Thumb::setProgress(float p)
{
	Size sz = m_area.getSize();
	float width = m_trackarea.getWidth();
	float height = m_trackarea.getHeight();
	point pt(m_trackarea.getPosition());
	point z(pt);

	if(m_horiz)
	{
		if(width > 0.f)
			pt.x += p * (width - sz.width);
		if(pt.x < z.x)
			pt.x = z.x;
		if(pt.x > z.x + width - sz.width)
			pt.x = z.x + width - sz.width;
	}
	else
	{
		if(height > 0.f)
			pt.y += p * (height - sz.height);
		if(pt.y < z.y)
			pt.y = z.y;
		if(pt.y > z.y + height - sz.height)
			pt.y = z.y + height - sz.height;
	}
	invalidate();
	m_area.setPosition(pt);
	send_event(events::TrackEvent());
}

void Thumb::setTrackarea(const Rect& rc)
{
	float progress = getProgress();
	m_trackarea = rc;
	setProgress(progress);
	invalidate();
}

void Thumb::render(const Rect& finalRect, const Rect& finalClip)
{
	ImageButton::render(finalRect, finalClip);
}

void Thumb::init(xml::node& node)
{
	ImageButton::init(node);
	
	xml::node setting = node("TrackArea");
	if(!setting.empty())
	{
		setTrackarea(StringToArea(setting.first_child().value()));
	}

	setting = node("Horizontal");
	if(!setting.empty())
	{
		m_horiz = StringToBool(setting.first_child().value());
	}

	setting = node("Progress");
	if(!setting.empty())
	{
		setProgress((float)atof(setting.first_child().value()));
	}

	setting = node("Movable");
	if(!setting.empty())
	{
		m_movable = StringToBool(setting.first_child().value());
	}
}

ScrollThumb::ScrollThumb(System& sys, const std::string& name) :
	Thumb(sys, name)
{
}

ScrollThumb::~ScrollThumb(void)
{
}

void ScrollThumb::render(const Rect& finalRect, const Rect& finalClip)
{
	States st = m_state;
/*	if(m_pushed) st = Pushed;
	else if(m_hovered || m_focus) st = Hovered;
	else st = Normal;*/	
	StateImagery& state = m_states[st];
	float left = 0;
	float right = 0;
	Rect componentRect;
	Size imgSize;

	Renderer& r = m_system.getRenderer();

	if(m_horiz)
	{
		// left image
		if (state.leftImg)
		{
			// calculate final destination area
			imgSize = state.leftImg->size();
			componentRect.m_left = finalRect.m_left;
			componentRect.m_top  = finalRect.m_top;
			componentRect.setSize(imgSize);
			componentRect = finalRect.getIntersection (componentRect);
			left  = imgSize.width;

			// draw this element.
			r.draw(*state.leftImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
		}
		// right image
		if (state.rightImg)
		{
			imgSize = state.rightImg->size();
			componentRect.m_left = finalRect.m_right - imgSize.width;
			componentRect.m_top  = finalRect.m_top;
			componentRect.setSize(imgSize);
			componentRect = finalRect.getIntersection (componentRect);

			right = imgSize.width;

			// draw this element.
			r.draw(*state.rightImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
		}
		// center image
		if (state.backImg)
		{
			componentRect = finalRect;
			componentRect.m_left += left;
			componentRect.m_right -= right;
	        
			// draw this element.
			r.draw(*state.backImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Tile, ImageOps::Stretch);
		}
	}
	else
	{
		// top image
		if (state.leftImg)
		{
			// calculate final destination area
			imgSize = state.leftImg->size();
			componentRect.m_left = finalRect.m_left;
			componentRect.m_top  = finalRect.m_top;
			componentRect.setSize(imgSize);
			componentRect = finalRect.getIntersection (componentRect);
			left  = imgSize.height;

			// draw this element.
			r.draw(*state.leftImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
		}
		// bottom image
		if (state.rightImg)
		{
			imgSize = state.rightImg->size();
			componentRect.m_left = finalRect.m_left;
			componentRect.m_top  = finalRect.m_bottom - imgSize.height;
			componentRect.setSize(imgSize);
			componentRect = finalRect.getIntersection (componentRect);

			right = imgSize.height;

			// draw this element.
			r.draw(*state.rightImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
		}
		// center image
		if (state.backImg)
		{
			componentRect = finalRect;
			componentRect.m_top += left;
			componentRect.m_bottom -= right;
	        
			// draw this element.
			r.draw(*state.backImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Tile);
		}
	}
}

void ScrollThumb::init(xml::node& node)
{
	Thumb::init(node);
	Button::init(node);	
}

}
