#pragma once

#include <guilib/src/system.h>
#include <guilib/src/font.h>

#include <vector>

namespace gui
{
	// Forward declarations for font properties
	namespace FontProperties
	{
		class FreeTypePointSize;
		class FreeTypeAntialiased;
	}

	class Renderer;

	class FreeTypeFont : public Font
	{
	public:
		FreeTypeFont (const std::string& name, const std::string& filename, unsigned int size, Renderer& r);
		virtual ~FreeTypeFont();

		virtual void rasterize(utf32 start_codepoint, utf32 end_codepoint);

	protected:
		void drawGlyphToBuffer (argb_t* buffer, unsigned int buf_width);
		unsigned int getTextureSize (CodepointMap::const_iterator s, CodepointMap::const_iterator e);

		virtual void updateFont();
		void free();

	protected:
		typedef std::vector<Imageset*> ImagesetVector;
		ImagesetVector m_glyphImages;

		float m_ptSize;
		bool m_antiAliased;

		FT_Face m_fontFace;
		data_ptr m_font_data;
/*
		FT_Byte* m_fontData;
		FT_Long m_fontSize;
*/
	};
}