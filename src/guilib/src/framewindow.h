#pragma once

#include "panel.h"
#include "textformatting.h"

namespace gui
{
	class Font;
	typedef std::shared_ptr<Font> FontPtr;
	class Imageset;
	typedef std::shared_ptr<Imageset> ImagesetPtr;

	class  FrameWindow : public Panel
	{
	public:
		typedef FrameWindow Self;
		FrameWindow(System& sys, const std::string& name = std::string());
		virtual ~FrameWindow();

		static const char* GetType() { return "FrameWindow"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void rise();
		
		virtual void render(const Rect& finalRect, const Rect& finalClip);
		void setFont(const std::string& font);
		
		void setCaption(const std::string& text)
		{
			m_text = text;
			invalidate();
		}
		const std::string& getCaption() const { return m_text; }

		void setCaptionColor(const Color& col) 
		{
			m_captionColor = col; 
			invalidate();
		}
		const Color& getCaptionColor() const { return m_captionColor; }

		void setMovable(bool movable) { m_movable = movable; }
		bool getMovable() const { return m_movable; }

		void setClamp(bool clamp) { m_clampToScreen = clamp; }
		bool getClamp() const { return m_clampToScreen; }

		void setCaptionFormatting(TextFormatting fmt) 
		{
			m_format = fmt;
			invalidate();
		}
		TextFormatting getCaptionFormatting() const { return m_format; }

		virtual bool onMouseMove();
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onCaptureLost();

		virtual void init(xml::node& node);

	protected:
		ImagesetPtr m_imgset;
		FontPtr m_font;
		std::string m_text;
		TextFormatting m_format;
		Color m_captionColor;

		// caption imagery
		const Image*	m_captionLeftImg;
		const Image*	m_captionRightImg;
		const Image*	m_captionBackImg;

		bool m_tracking;
		point m_offset;

		bool m_movable;
		bool m_clampToScreen;
	};
}