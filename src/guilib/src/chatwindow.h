#pragma once

#include "panel.h"
#include "font.h"

namespace gui
{
	class ChatWindow : public Panel
	{
	public:
		typedef ChatWindow Self;
		ChatWindow(System& sys, const std::string& name = std::string());
		virtual ~ChatWindow();

		static const char* GetType() { return "ChatWindow"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void rise();

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		void setFont(const std::string& font);

		void setCaption(const std::string& text);
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

		virtual void init(xml::node& node);

		////////////////////////////////////////////////////////////////////////////////////////////////
		typedef std::function <void ()> EndCallback;
		void SetEndCallback( EndCallback callback)
		{
			m_endCallback = callback;
		}
		virtual bool onTick(float delta);
		void Show(const std::string& text);
		void Hide() 
		{
			setVisible(false);
			base_window::stopTick();
		}
		void SetPosition(const point& point);
		void SetVisible(bool visible) {base_window::setVisible(visible);}

		void SetMaxWidth(float value)  { m_maxWidth = value; }
		void SetMinWidth(float value)  { m_minWidth = value; }

		void SetMaxHeight(float value)  { m_maxHeight = value; }
		void SetMinHeight(float value)  { m_minHeight = value; }

		void SetTextOffset(float x, float y)
		{
			m_textOffsetX = x;
			m_textOffsetY = y;
		}
		void SetShowingTime(float time) { m_fMaxShowingTime = time; }

	protected:
		ImagesetPtr m_imgset;
		FontPtr m_font;
		std::string m_text;
		TextFormatting m_format;
		Color m_captionColor;

		float m_fShowingTime;
		float m_fMaxShowingTime;
		// caption imagery
		const Image*	m_captionLeftImg;
		const Image*	m_captionRightImg;
		const Image*	m_captionBackImg;

		bool m_tracking;
		point m_offset;

		bool m_movable;
		bool m_clampToScreen;
		
		EndCallback m_endCallback;

		float m_maxWidth;
		float m_maxHeight;
		float m_minWidth;
		float m_minHeight;
		float m_textOffsetX;
		float m_textOffsetY;

	};
}
