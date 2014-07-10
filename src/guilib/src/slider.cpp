#include "StdAfx.h"

#include "slider.h"

#include "system.h"
#include "eventtypes.h"
#include "windowmanager.h"
#include "renderer.h"
#include "utils.h"

namespace gui
{

	Slider::Slider(System& sys, const std::string& name) : 
		base_window(sys, name),
		m_thumb(0),
		m_pos(0.f),
		m_step(0.f),
		m_doc(1.f)
	{
	}

	Slider::~Slider(void)
	{
	}

	float Slider::getScrollPosition() const
	{
		return m_pos;
	}

	void Slider::setScrollPosition(float p)
	{
		m_pos = p;
		if(m_thumb)
		{
			m_thumb->setProgress(p);
		}
		invalidate();
	}

	float Slider::getDocumentSize() const
	{
		return m_doc;
	}

	void Slider::setDocumentSize(float p)
	{
		m_doc = p;

		updateThumb();
	}

	bool Slider::onLoad(void)
	{
		window_ptr thumb = child("thumb");
		if(thumb)
		{
			m_thumb = dynamic_cast<Thumb*>(thumb.get());
			if(m_thumb)
			{
				subscribe<events::TrackEvent, Slider> (&Slider::onTrack, m_thumb);
				updateThumb();
			}
		}

		return base_window::onLoad();
	}

	bool Slider::onSized(bool update)
	{
		updateThumb();

		return base_window::onSized();
	}

	void Slider::updateThumb()
	{
		if(!m_thumb)
			return;
		m_thumb->setTrackarea(Rect(point(0.f, 0.f), m_area.getSize()));
		m_pos = m_thumb->getProgress();
	}

	void Slider::onTrack(const events::TrackEvent& e)
	{
		if(m_thumb)
		{
			m_pos = m_thumb->getProgress();

			EventArgs e;
			e.name = "On_Tracked";
			callHandler(&e);
			
			send_event(events::TrackEvent());
			invalidate();
		}
	}

	void Slider::init(xml::node& node)
	{
		base_window::init(node);
	}

	ScrollBar::ScrollBar(System& sys, const std::string& name) : 
		Slider(sys, name),
		m_incBtn(0),
		m_decBtn(0),
		m_backImg(0),
		m_leftImg(0),
		m_rightImg(0)
	{
	}

	ScrollBar::~ScrollBar(void)
	{
	}

	bool ScrollBar::onLoad(void)
	{
		window_ptr c = child("dec");
		if(c)
		{
			m_decBtn = dynamic_cast<Button*>(c.get());
			if(m_decBtn)
				subscribe<events::ClickEvent, ScrollBar> (&ScrollBar::onDecClick, m_decBtn);
		}
		
		c = child("inc");
		if(c)
		{
			m_incBtn = dynamic_cast<Button*>(c.get());
			if(m_incBtn)
				subscribe<events::ClickEvent, ScrollBar> (&ScrollBar::onIncClick, m_incBtn);
		}
		
		return Slider::onLoad();
	}

	void ScrollBar::updateThumb()
	{
		if(!m_thumb)
			return;
		float dmin = m_horiz ? m_area.getWidth() : m_area.getHeight();
		if(m_doc < dmin)
			m_doc = dmin;

		Rect thumbArea(point(0.f, 0.f), m_area.getSize());
		Size ibtnsize(0.f, 0.f);
		Size dbtnsize(0.f, 0.f);
		if(m_incBtn)
			ibtnsize = m_incBtn->getArea().getSize();
		if(m_decBtn)
			dbtnsize = m_decBtn->getArea().getSize();

		Size s(m_thumb->getArea().getSize());
		if(m_horiz)
		{
			thumbArea.m_left += dbtnsize.width;
			thumbArea.m_right -= ibtnsize.width;
			s.width = thumbArea.getWidth() * m_area.getWidth() / m_doc;
		}
		else
		{
			thumbArea.m_top += dbtnsize.height;
			thumbArea.m_bottom -= ibtnsize.height;
			s.height = thumbArea.getHeight() * m_area.getHeight() / m_doc;
		}
		m_thumb->setSize(s);
		m_thumb->setTrackarea(thumbArea);	
		m_thumb->setProgress(m_pos);
	}

	bool ScrollBar::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		if(btn == EventArgs::Left)
		{
			if(state == EventArgs::Down)
			{
				point pt = transformToWndCoord(m_system.getCursor().getPosition());
				pt -= m_area.getPosition();
				if(m_thumb)
				{
					const Rect& track = m_thumb->getTrackarea();
					float size = m_horiz ? track.getWidth() : track.getHeight();
					if(track.isPointInRect(pt))
					{
						point pos = pt - track.getPosition();
						/*pos.x -= m_thumb->getArea().getWidth() / 2;
						pos.y -= m_thumb->getArea().getHeight() / 2;*/
						m_thumb->setProgress(m_horiz ? pos.x / size : pos.y / size);
					}
				}
			}
		}
		return true;
	}

	bool ScrollBar::onMouseWheel(int delta)
	{
		if(delta < 0)
			onIncClick(events::ClickEvent());
		else if(delta > 0)
			onDecClick(events::ClickEvent());

		return true;
	}

	void ScrollBar::onIncClick(const events::ClickEvent& e) 
	{
		m_pos += m_step;
		if(m_pos > 1.f)
			m_pos = 1.f;
		m_thumb->setProgress(m_pos);
	}

	void ScrollBar::onDecClick(const events::ClickEvent& e) 
	{
		m_pos -= m_step;
		if(m_pos < 0.f)
			m_pos = 0.f;
		m_thumb->setProgress(m_pos);
	}


	void ScrollBar::render(const Rect& finalRect, const Rect& finalClip)
	{
		float left = 0;
		float right = 0;
		Rect componentRect(finalRect);
		Size imgSize;

		Renderer& r = m_system.getRenderer();

		if(m_horiz)
		{
			// left image
			if (m_leftImg)
			{
				// calculate final destination area
				imgSize = m_leftImg->size();
				componentRect.m_left = finalRect.m_left;
				componentRect.m_top  = finalRect.m_top;
				componentRect.setSize(imgSize);
				componentRect = finalRect.getIntersection (componentRect);
				left  = imgSize.width;

				// draw this element.
				r.draw(*m_leftImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
			}
			// right image
			if (m_rightImg)
			{
				imgSize = m_rightImg->size();
				componentRect.m_left = finalRect.m_right - imgSize.width;
				componentRect.m_top  = finalRect.m_top;
				componentRect.setSize(imgSize);
				componentRect = finalRect.getIntersection (componentRect);

				right = imgSize.width;

				// draw this element.
				r.draw(*m_rightImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
			}
			// center image
			if (m_backImg)
			{
				componentRect = finalRect;
				componentRect.m_left += left;
				componentRect.m_right -= right;
		        
				// draw this element.
				r.draw(*m_backImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Tile, ImageOps::Stretch);
			}
		}
		else
		{
			// top image
			if (m_leftImg)
			{
				// calculate final destination area
				imgSize = m_leftImg->size();
				componentRect.m_left = finalRect.m_left;
				componentRect.m_top  = finalRect.m_top;
				componentRect.setSize(imgSize);
				componentRect = finalRect.getIntersection (componentRect);
				left  = imgSize.height;

				// draw this element.
				r.draw(*m_leftImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
			}
			// bottom image
			if (m_rightImg)
			{
				imgSize = m_rightImg->size();
				componentRect.m_left = finalRect.m_left;
				componentRect.m_top  = finalRect.m_bottom - imgSize.height;
				componentRect.setSize(imgSize);
				componentRect = finalRect.getIntersection (componentRect);

				right = imgSize.height;

				// draw this element.
				r.draw(*m_rightImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
			}
			// center image
			if (m_backImg)
			{
				componentRect = finalRect;
				componentRect.m_top += left;
				componentRect.m_bottom -= right;
		        
				// draw this element.
				r.draw(*m_backImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Tile);
			}
		}
	}

	void ScrollBar::init(xml::node& node)
	{
		Slider::init(node);
		
		xml::node setting = node("Step");
		if(!setting.empty())
		{
			m_step = (float)atof(setting.first_child().value());
		}
		
		setting = node("Horizontal");
		if(!setting.empty())
		{
			m_horiz = StringToBool(setting.first_child().value());
		}

		setting = node("Document");
		if(!setting.empty())
		{
			setDocumentSize((float)atof(setting.first_child().value()));
		}
		xml::node frame = node("Frame");
		if(!frame.empty())
		{
			std::string setname = frame["Imageset"].value();
			m_imgset = m_system.getWindowManager().loadImageset(setname);
			if(m_imgset)
			{
				const Imageset& set = *m_imgset;
				m_backImg = set[frame("Background")["Image"].value()];
				m_leftImg = set[frame("Left")["Image"].value()];
				m_rightImg = set[frame("Right")["Image"].value()];
			}
		}
	}
}