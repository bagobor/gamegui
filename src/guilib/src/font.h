#pragma once

#include "colorRect.h"
#include "textformatting.h"
#include "imagesetmanager.h"

namespace gui
{
	class Image;
	class Imageset;
	class Renderer;

	typedef unsigned int utf32;

	class FontGlyph
	{
	public:
		FontGlyph (){}
		FontGlyph (float advance) : m_image (0), m_advance (advance), m_isLoaded(false), m_xoffset(0.f), m_yoffset(0.f), m_rect(0.f, 0.f, 0.f, 0.f) {}    

		const Image* getImage () const { return m_image; }
		//const Imageset* getImageset () const { return m_image->getImageset (); }

		void SetSize(float w, float h) {m_width = w; m_height = h;}

		Size getSize (float x_scale, float y_scale) const 
		{ 
			return Size(m_width*x_scale ,m_height*y_scale); 
		}

		float getWidth (float x_scale) const { return m_image->size().width * x_scale; }
		float getHeight(float y_scale) const { return m_image->size().height * y_scale; }
		float getRenderedAdvance(float x_scale) const { return (m_image->size().width + getOffsetX()) * x_scale; }
		float getAdvance (float x_scale = 1.0) const { return m_advance * x_scale; }

		float getOffsetX() const { return m_xoffset; }
		float getOffsetY() const { return m_yoffset; }
		void setOffsetX(float x) { m_xoffset = x; }
		void setOffsetY(float y) { m_yoffset = y; }

		void setRect(Rect& rc) { m_rect = rc; }
		const Rect& getRect() const { return m_rect; }

		const bool isLoaded() const { return m_isLoaded; }
		void setLoaded() { m_isLoaded = true; }
		void setAdvance (float advance) { m_advance = advance; }
		void setImage (const Image* image) { m_image = image; }
#ifdef GUI_KERNING_ENABLED
		void	addKerning(utf32 charcode, float kernvalue)
		{
			m_kerning[charcode] = kernvalue;
		}

		float	getKerning(utf32 charcode) const
		{
			KerningMap::const_iterator kern_it = m_kerning.find(charcode);
			return kern_it == m_kerning.end() ? 0.0f : (*kern_it).second;
		}
#endif
	private:
		const Image* m_image;
		float m_advance;
		float m_xoffset;
		float m_yoffset;
		Rect m_rect;
		bool m_isLoaded;

#ifdef GUI_KERNING_ENABLED
		typedef std::map<utf32, float> KerningMap;
		KerningMap	m_kerning;
#endif
		// size in pixels
		float m_width, m_height;
	};

	class  Font
	{
	public:
		/// Color value used whenever a color is not specified.
		static const argb_t DefaultColour;
		static const std::wstring	DefaultWhitespace;		//!< The default set of whitespace
		static const std::wstring	DefaultAlphanumerical;	//!< default set of alphanumerical.
		static const std::wstring	DefaultWrapDelimiters;	//!< The default set of word-wrap delimiters

		virtual ~Font ();

		bool isCodepointAvailable (utf32 cp) const
		{ 
			CodepointMapIndex::const_iterator it = m_cp_map_index.find(cp);
			return (it != m_cp_map_index.end()) ? it->second != m_cp_map.end() : false;
		}

		size_t drawText (const std::string& text, const Rect& draw_area, float z, const Rect& clip_rect, 
						TextFormatting fmt, const ColorRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

		size_t drawText (const std::wstring& text, const Rect& draw_area, float z, const Rect& clip_rect, 
						 TextFormatting fmt, const ColorRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

		size_t drawText (const std::string& text, const Rect& draw_area, float z, const Rect& clip_rect, 
							TextFormatting fmt, float x_scale = 1.0f, float y_scale = 1.0f)
		{ 
			return drawText (text, draw_area, z, clip_rect, fmt, ColorRect(DefaultColour), x_scale, y_scale); 
		}

		void drawText (const std::string& text, const Rect& draw_area, float z, const Rect& clip_rect, float x_scale = 1.0f, float y_scale = 1.0f)
		{ 
			drawText (text, draw_area, z, clip_rect, LeftAligned, ColorRect(DefaultColour), x_scale, y_scale);
		}

		size_t drawText (const std::string& text, const Rect& draw_area, float z, TextFormatting fmt, const ColorRect& colours, float x_scale = 1.0f, float y_scale = 1.0f)
		{ 
			return drawText (text, draw_area, z, draw_area, fmt, colours, x_scale, y_scale);
		}

		size_t drawText (const std::string& text, const Rect& draw_area, float z, TextFormatting fmt, float x_scale = 1.0f, float y_scale = 1.0f)
		{ 
			return drawText (text, draw_area, z, draw_area, fmt, ColorRect(DefaultColour), x_scale, y_scale);
		}

		void drawText (const std::string& text, const Rect& draw_area, float z, float x_scale = 1.0f, float y_scale = 1.0f)
		{ 
			drawText (text, draw_area, z, draw_area, LeftAligned, ColorRect(DefaultColour), x_scale, y_scale); 
		}

		void drawText (const std::string& text, const vec3& position, const Rect& clip_rect, const ColorRect& colours, float x_scale = 1.0f, float y_scale = 1.0f)
		{ 
			drawText (text, Rect (position.x, position.y, position.x, position.y), position.z, clip_rect, LeftAligned, colours, x_scale, y_scale);
		}

		void drawText (const std::string& text, const vec3& position, const Rect& clip_rect, float x_scale = 1.0f, float y_scale = 1.0f)
		{ 
			drawText (text, Rect (position.x, position.y, position.x, position.y), position.z, clip_rect, LeftAligned, ColorRect(DefaultColour), x_scale, y_scale); 
		}

		void setSpacing(float spacing) { m_spacing = spacing; }
		float getSpacing() const { return m_spacing; }

		float getLineSpacing (float y_scale = 1.0f) const { return m_height * y_scale * getSpacing(); }
		float getFontHeight (float y_scale = 1.0f) const { return (m_ascender - m_descender) * y_scale; }
		float getBaseline (float y_scale = 1.0f) const { return m_ascender * y_scale; }
		float getTextExtent (const std::wstring& text, float x_scale = 1.0f);
		float getTextExtent (const std::string& text, float x_scale = 1.0f);

		size_t getCharAtPixel (const std::string& text, float pixel, float x_scale = 1.0f);
		size_t getCharAtPixel (const std::wstring& text, float pixel, float x_scale = 1.0f) { return getCharAtPixel(text, 0, pixel, x_scale); }

		size_t getCharAtPixel (const std::wstring& text, size_t start_char, float pixel, float x_scale = 1.0f);

		size_t getFormattedLineCount(const std::string& text, const Rect& format_area, TextFormatting fmt, float x_scale = 1.0f);
		size_t getFormattedLineCount (const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale = 1.0f);

		float getFormattedTextExtent (const std::string& text, const Rect& format_area, TextFormatting fmt, float x_scale = 1.0f);
		float getFormattedTextExtent (const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale = 1.0f);

		const std::string&	getName(void) const {return m_name;}

	protected:
		Font (const std::string& name, const std::string& fontname, unsigned int size, Renderer& render);
		Font& operator=(const Font&);

		virtual void updateFont () = 0;

		size_t drawWrappedText (const std::wstring& text, const Rect& draw_area, float z, const Rect& clip_rect, 
								TextFormatting fmt, const ColorRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

		size_t getNextWord (const std::wstring& in_string, size_t start_idx, size_t max_length, std::wstring& out_string) const;

		void drawTextLine (const std::wstring& text, const Rect& draw_area, const vec3& position, const Rect& clip_rect, 
							const ColorRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

		void drawTextLineJustified (const std::wstring& text, const Rect& draw_area, const vec3& position, 
							const Rect& clip_rect, const ColorRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

		float getWrappedTextExtent (const std::wstring& text, float wrapWidth, float x_scale = 1.0f);

		const FontGlyph* getGlyphData (utf32 codepoint);

		void setMaxCodepoint (utf32 codepoint);

		virtual void rasterize (utf32 start_codepoint, utf32 end_codepoint);

	protected:
		typedef std::map<utf32, FontGlyph> CodepointMap;
		/// Contains mappings from code points to Image objects
		CodepointMap m_cp_map;
		typedef std::unordered_map<utf32, CodepointMap::const_iterator> CodepointMapIndex;
		CodepointMapIndex m_cp_map_index;

		std::string m_name;
		std::string m_fileName;

		/// maximal font ascender (pixels above the baseline)
		float m_ascender;
		/// maximal font descender (negative pixels below the baseline)
		float m_descender;
		/// (ascender - descender) + linegap
		float m_height;

		float m_spacing;

		/// Maximal codepoint for font glyphs
		utf32 m_maxCodepoint;

		unsigned int *m_glyphPageLoaded;

		Renderer& m_render;
		unsigned int m_size;
	};

	typedef std::shared_ptr<Font> FontPtr;

	TextFormatting StringToFormatType(const std::string& type);
}