#pragma once
#include "window.h"
#include "textformatting.h"

namespace gui
{
	class Font;
	typedef std::shared_ptr<Font> FontPtr;

	class  Label :	public WindowBase
	{
	public:
		typedef Label self_t;
		Label(System& sys, const std::string& name = std::string());
		virtual ~Label();

		static const char* GetType() { return "Label"; }
		virtual const char* getType() const { return self_t::GetType(); }

		void setFont(const std::string& font);
		const std::string&	getFontName() const;

		void setFont(FontPtr font) { m_font = font; }
		
		virtual void setText(const std::string& text);
		virtual const std::string getText() const { return m_text; }
		virtual void appendText(const std::string& text);

		void setTextFormatting(TextFormatting fmt)
		{
			m_format = fmt;
			invalidate();
		}
		TextFormatting getTextFormatting() const { return m_format; }

		void setSpacing(float spacing);
		float getSpacing() const { return m_spacing; }

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		virtual void init(xml::node& node);

	protected:
		FontPtr m_font;
		std::string m_text;
		TextFormatting m_format;
		float m_spacing;

		point m_shadow_offset;
		Size m_shadow_scale;

		bool m_shadow_enabled;
		
		bool m_centred;
	};
}
