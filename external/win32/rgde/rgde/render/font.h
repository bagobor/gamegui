#pragma once

#include <rgde/core/math/forward.h>

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace rgde
{
	namespace render
	{
		class device;

		class font
		{
			class font_impl;
			typedef boost::scoped_ptr<font_impl> pimpl;
		public:
			enum weight
			{
				thin,
				extra_light,
				light,
				normal,
				medium,
				semi_bold,
				bold,
				extra_bold,
				heavy
			};

			enum format
			{				
				top = 0,				// Top-justifies text.
				left = 0,					// Aligns text to the left. 
				center = 0x00000001,		// Centers text horizontally in the rectangle.
				right = 0x00000002,			// Aligns text to the right. 
				vcenter = 0x00000004,		// Centers text vertically (single line only). 
				bottom = 0x00000008,		// Justifies the text to the bottom of the rectangle. This value must be 
											// combined with SingleLine.

				word_break = 0x00000010,	// Breaks words. Lines are automatically broken between words if a word would 
											// extend past the edge of the rectangle specified by the pRect parameter. A 
											// carriage return/line feed sequence also breaks the line.

				single_line = 0x00000020,	// Displays text on a single line only. Carriage returns and line feeds do not 
											// break the line. 
				no_clip = 0x00000100,		// Draws without clipping. IFont::renderText is somewhat faster when NoClip is 
			};

			static font_ptr create(device& dev, size_t height, const std::wstring& name, weight w = normal);

			void render(const std::wstring& text, const math::rect& rc, unsigned int color = 0xFFFFFFFF);
			void render(const std::wstring& text, const math::rect& rc, unsigned int color, bool draw_shadow = true);
			void render(const std::wstring& text, const math::rect& rc, unsigned int color, bool draw_shadow, int flags);

			math::rect measure_text( const std::wstring& text, int flags = 0);

			void render(const std::string& text, const math::rect& rc, unsigned int color = 0xFFFFFFFF);
			void render(const std::string& text, const math::rect& rc, unsigned int color, bool draw_shadow = true);
			void render(const std::string& text, const math::rect& rc, unsigned int color, bool draw_shadow, int flags);

			void render(const std::string& text, float x, float y, unsigned int color);
			void render(const std::wstring& text, float x, float y, unsigned int color);

			math::rect measure_text( const std::string& text, int flags = 0);

			~font();

		private:
			font(device& dev, size_t height, const std::wstring& name, weight w);			

		private:
			pimpl m_pimpl;
		};
	}
}