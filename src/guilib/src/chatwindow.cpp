#include "stdafx.h"

#include "chatwindow.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"
#include "utils.h"

#include "../xml/xml.h"

namespace gui
{
	ChatWindow::ChatWindow(System& sys, const std::string& name)
		: Panel(sys, name)
		, m_captionLeftImg(0)
		, m_captionRightImg(0)
		, m_captionBackImg(0)
		, m_tracking(false)
		, m_movable(false)
		, m_clampToScreen(false)
		, m_captionColor(1.f, 1.f, 1.f)
	{
		m_area.m_left = 0;
		m_area.m_right = 100;
		m_area.m_top = 0;
		m_area.m_bottom = 100;
		m_fShowingTime = 0.0f;


		m_maxWidth = 200.0f;
		m_maxHeight = 200.0f;
		m_minWidth = 50.0f;
		m_minHeight = 50.0f;
		m_textOffsetX = 10.0f;
		m_textOffsetY = 10.0f;

		m_fMaxShowingTime = 4.0f;
	}

	ChatWindow::~ChatWindow(void)
	{
	}

	void ChatWindow::rise()
	{
		if(m_parent)
			m_parent->moveToFront(this);

		base_window::rise();
	}

	void ChatWindow::setFont(const std::string& font)
	{
		if(font.empty())
			return;

		m_font = m_system.getWindowManager().loadFont(font);
		invalidate();
	}

	void ChatWindow::render(const Rect& finalRect, const Rect& finalClip)
	{
		Renderer& r = m_system.getRenderer();

		renderFrame(finalRect, finalClip);

		float dx = 1.0f;
		float dy = 1.0f;

		Rect textRect (finalRect.m_left + m_textOffsetX, finalRect.m_top + m_textOffsetY, 
					   finalRect.m_right - m_textOffsetX + dx,  finalRect.m_bottom - m_textOffsetY + dy);

		if(m_font)
			m_font->drawText(m_text, textRect, 1.0f, finalClip, m_format, m_captionColor, 1.f, 1.f);
	}

	void ChatWindow::init(xml::node& node)
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
			std::string setname = frame["Imageset"].value();
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

	void ChatWindow::Show( const std::string& text )
	{

		Rect rect(0,0, m_maxWidth, 0);
		float scale_x = 1.0f;
		float scale_y = 1.0f;

		float w = m_font->getFormattedTextExtent(text, rect, m_format, scale_x);
		
		
		w = std::max(w, m_minWidth);
		rect.m_right = w;

		float lineH = m_font->getLineSpacing(scale_y);//+ m_font->getFontHeight(scale_y);

		float h = (float)m_font->getFormattedLineCount(text, rect, m_format, scale_y) * lineH;
		h = std::min(h, m_maxHeight);
		h = std::max(h, m_minHeight);

		m_text = text;

		m_area.m_left = -0.5f * w - m_textOffsetX;
		m_area.m_right = -m_area.m_left;
		m_area.m_top =  - h - m_textOffsetY;
		m_area.m_bottom =  m_textOffsetY;

		m_fShowingTime = 0.0f;
			
		base_window::startTick();
	}

	void ChatWindow::SetPosition( const point& point )
	{
		float w2 = m_area.getWidth() * 0.5f;
		float h = m_area.getHeight();

		m_area.m_left = point.x - w2;
		m_area.m_right = point.x + w2;
		m_area.m_top = point.y - h;
		m_area.m_bottom = point.y;
	}

	bool ChatWindow::onTick( float delta )
	{
		base_window::onTick(delta);
		m_fShowingTime += delta;
		if (m_fShowingTime > m_fMaxShowingTime)
		{
			base_window::stopTick();
			if (m_endCallback)
				m_endCallback();
		}

		return false;
	}

	void ChatWindow::setCaption( const std::string& text )
	{
		m_text = text;
		invalidate();
	}
}