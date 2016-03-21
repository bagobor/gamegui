#include "stdafx.h"

#include "framewindow.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"
#include "font.h"
#include "utils.h"

#include "../xml/xml.h"

namespace gui
{
	FrameWindow::FrameWindow(System& sys, const std::string& name) :
		Panel(sys, name),
		m_captionLeftImg(0),
		m_captionRightImg(0),
		m_captionBackImg(0),
		m_tracking(false),
		m_movable(true),
		m_clampToScreen(true),
		m_captionColor(0.f, 0.f, 0.f)
	{
	}

	FrameWindow::~FrameWindow(void)
	{
	}

	void FrameWindow::rise()
	{
		if (getDisableRise()) return;

		if(m_parent)
			m_parent->moveToFront(this);

		WindowBase::rise();
	}

	void FrameWindow::setFont(const std::string& font)
	{
		if(font.empty())
			return;

		m_font = m_system.getWindowManager().loadFont(font);

		invalidate();
	}

	bool FrameWindow::onMouseMove(void)
	{
		if (!m_tracking) return false;

		point pt = transformToWndCoord(m_system.getCursor().getPosition());
		point newpos = pt - m_offset;

		Rect testarea(m_area);
		testarea.setPosition(newpos);

		if (m_clampToScreen && m_parent)
		{
			Size me = m_area.getSize();
			Size max = m_parent->getSize();
			if (testarea.m_left < 0.f)
				testarea.m_left = 0.f;
			if (testarea.m_top < 0.f)
				testarea.m_top = 0.f;
			if (testarea.m_right > max.width)
				testarea.m_left = max.width - me.width;
			if (testarea.m_bottom > max.height)
				testarea.m_top = max.height - me.height;

			testarea.setSize(me);
		}

		setArea(testarea);

		EventArgs a;
		a.name = "On_Move";
		callHandler(&a);

		return false;
	}

	bool FrameWindow::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		if(state == EventArgs::Down && m_movable)
		{
			m_system.queryCaptureInput(this);
			m_tracking = true;

			point pt = transformToWndCoord(m_system.getCursor().getPosition());
			m_offset = pt - m_area.getPosition();
		}
		else
		{
			if(m_tracking)
				m_system.queryCaptureInput(0);

			m_tracking = false;
		}
		return WindowBase::onMouseButton(btn, state);
	}

	bool FrameWindow::onCaptureLost(void)
	{
		m_tracking = false;
		return true;
	}

	void FrameWindow::render(const Rect& finalRect, const Rect& finalClip)
	{
		Renderer& r = m_system.getRenderer();

		// rendering caption
		float left = 0;
		float right = 0;
		float height = 0;
		
		Size imgSize;

		if (m_captionLeftImg)
		{
			// calculate final destination area
			imgSize = m_captionLeftImg->size();
			Rect componentRect;
			componentRect.m_left = finalRect.m_left;
			componentRect.m_top  = finalRect.m_top;
			componentRect.setSize(imgSize);
			componentRect = finalRect.getIntersection (componentRect);
			left  = imgSize.width;
			height = imgSize.height;

			// draw this element.
			r.draw(*m_captionLeftImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
		}
		// right image
		if (m_captionRightImg)
		{
			imgSize = m_captionRightImg->size();
			Rect componentRect;
			componentRect.m_left = finalRect.m_right - imgSize.width;
			componentRect.m_top  = finalRect.m_top;
			componentRect.setSize(imgSize);
			componentRect = finalRect.getIntersection (componentRect);

			right = imgSize.width;

			// draw this element.
			r.draw(*m_captionRightImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
		}
		// center image
		if (m_captionBackImg)
		{
			Rect componentRect = finalRect;
			componentRect.m_left += left;
			componentRect.m_right -= right;
			componentRect.m_bottom = componentRect.m_top + height;
	        
			// draw this element.
			r.draw(*m_captionBackImg, componentRect, 1.f, finalClip, m_backColor, ImageOps::Tile, ImageOps::Stretch);
		}

		//rendering frame
		const Rect rc(finalRect.m_left, finalRect.m_top + height, finalRect.m_right, finalRect.m_bottom);
		renderFrame(rc, finalClip);
		
		if(m_font)
		{
			Rect componentRect = finalRect;
			componentRect.m_bottom = componentRect.m_top + height;
			float font_height = m_font->getFontHeight();
			float caption_height = componentRect.m_bottom - componentRect.m_top;
			componentRect.m_top += (caption_height - font_height)*0.5f;
			m_font->drawText(m_text, componentRect, 1.0f, finalClip, m_format, m_captionColor, 1.f, 1.f);
		}
	}

	void FrameWindow::init(xml::node& node)
	{
		Panel::init(node);

		xml::node setting = node("Title");
		if(!setting.empty())
		{
			m_text = setting.first_child().value();
		}

		setting = node("Font");
		if(!setting.empty())
		{
			m_font = m_system.getWindowManager().loadFont(setting.first_child().value());
		}

		setting = node("Formatting");
		if(!setting.empty())
		{
			m_format = StringToFormatType(setting.first_child().value());
		}

		setting = node("CaptionColor");
		if(!setting.empty())
		{
			m_captionColor = StringToColor(setting.first_child().value());
		}
		
		setting = node("Movable");
		if(!setting.empty())
		{
			m_movable = StringToBool(setting.first_child().value());
		}

		setting = node("ClampToScreen");
		if(!setting.empty())
		{
			m_clampToScreen = StringToBool(setting.first_child().value());
		}

		xml::node frame = node("Caption");
		if(!frame.empty())
		{
			std::string setname = frame["imageset"].value();
			m_imgset = m_system.getWindowManager().loadImageset(setname);
			if(m_imgset)
			{
				const Imageset& set = *m_imgset;
				m_captionBackImg = set[frame("Background")["Image"].value()];
				m_captionLeftImg = set[frame("Left")["Image"].value()];
				m_captionRightImg = set[frame("Right")["Image"].value()];
			}
		}
	}
}