#include "stdafx.h"
#include "font.h"

#include "renderer.h"
#include "utils.h"

// amount of bits in a unsigned int
#define BITS_PER_UINT   (sizeof (unsigned int) * 8)
// must be a power of two
#define GLYPHS_PER_PAGE 256
#define MAX_FONT_SIZE 256

namespace gui
{
	const argb_t Font::DefaultColour = 0xFFFFFFFF;

	const std::wstring Font::DefaultWhitespace(L" \n\t\r");
	const std::wstring Font::DefaultAlphanumerical(L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	const std::wstring Font::DefaultWrapDelimiters(L" \n\t\r");

	Font::Font (const std::string& name, const std::string& fontname, unsigned int size, Renderer& render) :
		m_name (name),
		m_fileName (fontname),
		m_ascender (0),
		m_descender (0),
		m_height (0),   
		m_maxCodepoint (0),
		m_glyphPageLoaded (0),
		m_render(render),
		m_spacing(1.f)
	{
		m_size = size;
		if(size < 1)
			m_size = 1;
		else if(size > MAX_FONT_SIZE)
			m_size = MAX_FONT_SIZE;

	}

	Font::~Font(void)
	{
		delete [] m_glyphPageLoaded;
	}

	void Font::setMaxCodepoint (utf32 codepoint)
	{
		m_maxCodepoint = codepoint;

		delete [] m_glyphPageLoaded;

		unsigned int npages = (codepoint + GLYPHS_PER_PAGE) / GLYPHS_PER_PAGE;
		unsigned int size = (npages + BITS_PER_UINT - 1) / BITS_PER_UINT;
		m_glyphPageLoaded = new unsigned int [size];
		memset (m_glyphPageLoaded, 0, size * sizeof (unsigned int));
	}

	const FontGlyph *Font::getGlyphData (utf32 codepoint)
	{
		if (codepoint > m_maxCodepoint)
			return 0;

		if (m_glyphPageLoaded)
		{
			// Check if glyph page has been rasterized
			unsigned int page = codepoint / GLYPHS_PER_PAGE;
			unsigned int mask = 1 << (page & (BITS_PER_UINT - 1));
			if (!(m_glyphPageLoaded [page / BITS_PER_UINT] & mask))
			{
				m_glyphPageLoaded [page / BITS_PER_UINT] |= mask;
				rasterize (codepoint & ~(GLYPHS_PER_PAGE - 1),
						   codepoint | (GLYPHS_PER_PAGE - 1));
			}
		}

		CodepointMapIndex::const_iterator pos = m_cp_map_index.find (codepoint);
		return (pos != m_cp_map_index.end()) ? &pos->second->second : 0;
	}

	float Font::getTextExtent(const std::wstring& text, float x_scale)
	{
		const FontGlyph* glyph;
		float cur_extent = 0, adv_extent = 0, width;

		const size_t size = text.length();
		for (size_t c = 0; c < size; ++c)
		{
			glyph = getGlyphData(text[c]);

			if (glyph)
			{
				width = glyph->getRenderedAdvance(x_scale);

				if (adv_extent + width > cur_extent)
					cur_extent = adv_extent + width;

				adv_extent += glyph->getAdvance(x_scale);
	#ifdef GUI_KERNING_ENABLED
				if( c < (size-1) )
					adv_extent += glyph->getKerning(text[c+1]) * x_scale;
	#endif
			}
		}

		return std::max(adv_extent, cur_extent);
	}

	size_t Font::getCharAtPixel (const std::string& text, float pixel, float x_scale)
	{
		return getCharAtPixel(UTF8ToUTF16(text), pixel, x_scale);
	}

	size_t Font::getCharAtPixel(const std::wstring& text, size_t start_char, float pixel, float x_scale)
	{
		const FontGlyph* glyph;
		float cur_extent = 0;
		size_t char_count = text.length();

		// handle simple cases
		if ((pixel <= 0) || (char_count <= start_char))
			return start_char;

		for (size_t c = start_char; c < char_count; ++c)
		{
			glyph = getGlyphData(text[c]);

			if (glyph)
			{
				cur_extent += glyph->getAdvance(x_scale);
	#ifdef GUI_KERNING_ENABLED
				if (c < text.length() - 1)
					cur_extent += glyph->getKerning(text[c+1]) * x_scale;
	#endif
				if (pixel < cur_extent)
					return c;
			}
		}

		return char_count;
	}

	float Font::getTextExtent(const std::string& text, float x_scale) 
	{ 
		return getTextExtent(UTF8ToUTF16(text),x_scale); 
	}

	size_t Font::getFormattedLineCount(const std::string& text, const Rect& format_area, TextFormatting fmt, float x_scale)
	{
		return getFormattedLineCount(UTF8ToUTF16(text), format_area, fmt, x_scale);
	}


	size_t Font::getFormattedLineCount(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale)
	{
		// handle simple non-wrapped cases.
		if ((fmt == LeftAligned) || (fmt == Centred) || (fmt == RightAligned) || (fmt == Justified))
		{
			return std::count(text.begin(), text.end(), '\n') + 1;
		}

		// handle wraping cases
		size_t lineStart = 0, lineEnd = 0;
		std::wstring	sourceLine;

		float	wrap_width = format_area.getWidth();
		std::wstring  whitespace = DefaultWhitespace;
		std::wstring	thisLine, thisWord;
		size_t	line_count = 0, currpos = 0;

		while (lineEnd < text.length())
		{
			if ((lineEnd = text.find_first_of('\n', lineStart)) == std::wstring::npos)
			{
				lineEnd = text.length();
			}

			sourceLine = text.substr(lineStart, lineEnd - lineStart);
			lineStart = lineEnd + 1;

			// get first word.
			currpos = getNextWord(sourceLine, 0, getCharAtPixel(sourceLine, 0, wrap_width, x_scale), thisLine);

			// while there are words left in the string...
			while (std::wstring::npos != sourceLine.find_first_not_of(whitespace, currpos))
			{
				// get next word of the string...
				size_t max_length = std::max((size_t)getCharAtPixel(sourceLine, currpos, wrap_width-getTextExtent(thisLine, x_scale), x_scale) - currpos, (size_t)1);
				currpos += getNextWord(sourceLine, currpos, max_length, thisWord);

				// if the new word would make the string too long
				if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
				{
					// too long, so that's another line of text
					line_count++;

					std::wstring::size_type offset = thisWord.find_first_not_of(whitespace);
					// remove whitespace from next word - it will form start of next line
					if (offset != std::wstring::npos)			
						thisWord = thisWord.substr(offset);
					else
						thisWord.clear();

					// reset for a new line.
					thisLine.clear();
				}

				// add the next word to the line
				thisLine += thisWord;
			}

			// plus one for final line
			line_count++;
		}

		return line_count;
	}

	size_t Font::drawText(const std::string& text, const Rect& draw_area, float z, const Rect& clip_rect, TextFormatting fmt, const ColorRect& colours, float x_scale, float y_scale)
	{
		return drawText(UTF8ToUTF16(text), draw_area, z, clip_rect, fmt, colours, x_scale, y_scale);
	}

	size_t Font::drawText(const std::wstring& text, const Rect& draw_area, float z, const Rect& clip_rect, TextFormatting fmt, const ColorRect& colours, float x_scale, float y_scale)
	{
		size_t thisCount;
		size_t lineCount = 0;

		float	y_base = draw_area.m_top + getBaseline(y_scale);

		Rect tmpDrawArea = draw_area;

		size_t lineStart = 0, lineEnd = 0;
		std::wstring currLine;

		while (lineEnd < text.length())
		{
			if ((lineEnd = text.find_first_of(L"\n", lineStart)) == std::wstring::npos)
				lineEnd = text.length();

			currLine = text.substr(lineStart, lineEnd - lineStart);
			lineStart = lineEnd + 1;	// +1 to skip \n char

			switch(fmt)
			{
			case LeftAligned:
				drawTextLine(currLine, tmpDrawArea, Vector3(tmpDrawArea.m_left, y_base, z), clip_rect, colours, x_scale, y_scale);
				thisCount = 1;
				y_base += getLineSpacing(y_scale);
				break;

			case RightAligned:
				drawTextLine(currLine, tmpDrawArea, Vector3(tmpDrawArea.m_right - getTextExtent(currLine, x_scale), y_base, z), clip_rect, colours, x_scale, y_scale);
				thisCount = 1;
				y_base += getLineSpacing(y_scale);
				break;

			case Centred:
				drawTextLine(currLine, tmpDrawArea, Vector3((tmpDrawArea.m_left + ((tmpDrawArea.getWidth() - getTextExtent(currLine, x_scale)) / 2.0f)), y_base, z), clip_rect, colours, x_scale, y_scale);
				thisCount = 1;
				y_base += getLineSpacing(y_scale);
				break;

			case Justified:
				// new function in order to keep drawTextLine's signature unchanged
				drawTextLineJustified(currLine, draw_area, Vector3(tmpDrawArea.m_left, y_base, z), clip_rect, colours, x_scale, y_scale);
				thisCount = 1;
				y_base += getLineSpacing(y_scale);
				break;

			case WordWrapLeftAligned:
				thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, LeftAligned, colours, x_scale, y_scale);
				tmpDrawArea.m_top += thisCount * getLineSpacing(y_scale);
				break;

			case WordWrapRightAligned:
				thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, RightAligned, colours, x_scale, y_scale);
				tmpDrawArea.m_top += thisCount * getLineSpacing(y_scale);
				break;

			case WordWrapCentred:
				thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, Centred, colours, x_scale, y_scale);
				tmpDrawArea.m_top += thisCount * getLineSpacing(y_scale);
				break;

			case WordWrapJustified:
				// no change needed
				thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, Justified, colours, x_scale, y_scale);
				tmpDrawArea.m_top += thisCount * getLineSpacing(y_scale);
				break;

			default:
				throw std::exception("Font::drawText - Unknown or unsupported TextFormatting value specified.");
			}

			lineCount += thisCount;

		}

		// should not return 0
		return std::max(lineCount, (size_t)1);
	}

	size_t Font::getNextWord(const std::wstring& in_string, size_t start_idx, size_t max_length, std::wstring& out_string) const
	{
		std::wstring::size_type	word_start = in_string.find_first_not_of(DefaultWrapDelimiters, start_idx);

		if (word_start == std::wstring::npos)
		{
			word_start = start_idx;
		}

		std::wstring::size_type	word_end = in_string.find_first_of(DefaultWrapDelimiters, word_start);

		if (word_end == std::wstring::npos)
		{
			word_end = in_string.length();
			if ((word_end - start_idx) > max_length)
				word_end = start_idx + max_length;
		}

		out_string = in_string.substr(start_idx, (word_end - start_idx));

		return out_string.length();
	}

	size_t Font::drawWrappedText(const std::wstring& text, const Rect& draw_area, float z, const Rect& clip_rect, TextFormatting fmt, const ColorRect& colours, float x_scale, float y_scale)
	{
		if(Rect(draw_area).getIntersection(clip_rect).empty())
			return 0;

		size_t	line_count = 0;
		Rect	dest_area(draw_area);
		float	wrap_width = draw_area.getWidth();

		std::wstring  whitespace = DefaultWhitespace;
		std::wstring	thisLine, thisWord;
		size_t	currpos = 0;

		// get first word.
		currpos += getNextWord(text, 0, getCharAtPixel(text, 0, wrap_width, x_scale), thisLine);

		// while there are words left in the string...
		while (std::wstring::npos != text.find_first_not_of(whitespace, currpos))
		{
			// get next word of the string...
			size_t max_length = std::max((size_t)getCharAtPixel(text, currpos, wrap_width-getTextExtent(thisLine, x_scale), x_scale) - currpos, (size_t)1);
			currpos += getNextWord(text, currpos, max_length, thisWord);

			// if the new word would make the string too long
			if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
			{
				// output what we had until this new word
				line_count += drawText(thisLine, dest_area, z, clip_rect, fmt, colours, x_scale, y_scale);

				std::wstring::size_type offset = thisWord.find_first_not_of(whitespace);
				// remove whitespace from next word - it will form start of next line
				if (offset != std::wstring::npos)			
					thisWord = thisWord.substr(offset);
				else
					thisWord.clear();

				// reset for a new line.
				thisLine.clear();

				// update y co-ordinate for next line
				dest_area.m_top += getLineSpacing(y_scale);
			}

			// add the next word to the line
			thisLine += thisWord;
		}

		// Last line is left aligned
		TextFormatting last_fmt = (fmt == Justified ? LeftAligned : fmt);
		// output last bit of string
		line_count += drawText(thisLine, dest_area, z, clip_rect, last_fmt, colours, x_scale, y_scale);

		return line_count;
	}

	//#define PixelAligned(x)	( ( (float)(int)(( x ) + (( x ) > 0.0f ? 0.5f : -0.5f)) ) )

	void Font::drawTextLine(const std::wstring& text,  const Rect& draw_area, const Vector3& position, const Rect& clip_rect, const ColorRect& colours, float x_scale, float y_scale)
	{
		if(Rect(draw_area).getIntersection(clip_rect).empty())
			return;

		//{ // draw text shadow
		//	Vector3	cur_pos(position);

		//	float base_y = position.y;

		//	for (size_t c = 0; c < text.length(); ++c)
		//	{ 
		//		if (const FontGlyph* glyph = getGlyphData(text[c]))
		//		{
		//			const Image* img = glyph->getImage();
		//            
		//			Size size = glyph->getSize(x_scale, y_scale);
		//			float x = cur_pos.x + glyph->getOffsetX() * x_scale;					  
		//			float y = cur_pos.y + glyph->getOffsetY() * y_scale;

		//			Rect rec (x, y, x + size.width, y + size.height);

		//			Rect shadow_rec (rec.m_left+.5f,rec.m_top+.5f,rec.m_right+1.f,rec.m_bottom+1.f);
		//			Color shadow_color(0,0,0,colours.m_top_left.getAlpha());
		//			ColorRect shadow_colours(shadow_color,shadow_color,
		//										shadow_color,shadow_color);
		//			
		//			m_render.draw(*img, shadow_rec, cur_pos.z, clip_rect, shadow_colours);
		//			//m_render.draw(*img, rec, cur_pos.z, clip_rect, shadow_colours);

		//			cur_pos.x += glyph->getAdvance(x_scale);
		//#ifdef GUI_KERNING_ENABLED
		//			if (c < text.length() - 1)
		//				cur_pos.x += glyph->getKerning(text[c+1]) * x_scale;
		//#endif
		//		}
		//	}
		//}

		Vector3	cur_pos(position);

		for (size_t c = 0; c < text.length(); ++c)
		{ 
			if (const FontGlyph* glyph = getGlyphData(text[c]))
			{
				const Image* img = glyph->getImage();

				Size size = glyph->getSize(x_scale, y_scale);
				float x = cur_pos.x + glyph->getOffsetX() * x_scale;					  
				float y = cur_pos.y + glyph->getOffsetY() * y_scale;

				Rect rec (x, y, x + size.width, y + size.height);
				m_render.draw(*img, rec, cur_pos.z, clip_rect, colours);

				cur_pos.x += glyph->getAdvance(x_scale);
	#ifdef GUI_KERNING_ENABLED
				if (c < text.length() - 1)
					cur_pos.x += glyph->getKerning(text[c+1]) * x_scale;
	#endif
			}
		}
	}

	void Font::drawTextLineJustified (const std::wstring& text, const Rect& draw_area, const Vector3& position, const Rect& clip_rect, const ColorRect& colours, float x_scale, float y_scale)
	{
		if(Rect(draw_area).getIntersection(clip_rect).empty())
			return;

	/*	{
			Vector3	cur_pos(position);

			const FontGlyph* glyph;
			float base_y = position.y;
			size_t char_count = text.length();

			// Calculate the length difference between the justified text and the same text, left aligned
			// This space has to be shared between the space characters of the line
			float lost_space = getFormattedTextExtent(text, draw_area, Justified, x_scale) - getTextExtent(text, x_scale);

			// The number of spaces and tabs in the current line
			unsigned int space_count = 0;
			size_t c;
			for (c = 0; c < char_count; ++c)
				if ((text[c] == ' ') || (text[c] == '\t'))
					++space_count;

			// The width that must be added to each space character in order to transform the left aligned text in justified text
			float shared_lost_space = 0.0;
			if (space_count > 0)
				shared_lost_space = lost_space / (float)space_count;

			for (c = 0; c < char_count; ++c)
			{
				glyph = getGlyphData(text[c]);

				if (glyph)
				{
					const Image* img = glyph->getImage();
					Size size = glyph->getSize(x_scale, y_scale);
					Rect rec (cur_pos.x + glyph->getOffsetX() * x_scale, cur_pos.y + glyph->getOffsetY() * y_scale, cur_pos.x + glyph->getOffsetX() * x_scale + size.width, cur_pos.y + glyph->getOffsetY() * y_scale + size.height);

					Rect shadow_rec (rec.m_left+.5f,rec.m_top+.5f,rec.m_right+1.f,rec.m_bottom+1.f);
					ColorRect shadow_color (Color(0,0,0,colours.m_top_left.getAlpha()),Color(0,0,0,colours.m_top_right.getAlpha()),Color(0,0,0,colours.m_bottom_left.getAlpha()),Color(0,0,0,colours.m_bottom_right.getAlpha()));
					m_render.draw(*img, shadow_rec, cur_pos.z, clip_rect, shadow_color);

					cur_pos.x += glyph->getAdvance(x_scale);
	#ifdef GUI_KERNING_ENABLED
					if (c < text.length() - 1)
						cur_pos.x += glyph->getKerning(text[c+1]) * x_scale;
	#endif
					// That's where we adjust the size of each space character
					if ((text[c] == ' ') || (text[c] == '\t'))
						cur_pos.x += shared_lost_space;
				}
			}
		}*/

		{
			Vector3	cur_pos(position);

			const FontGlyph* glyph;
			float base_y = position.y;
			size_t char_count = text.length();

			// Calculate the length difference between the justified text and the same text, left aligned
			// This space has to be shared between the space characters of the line
			float lost_space = getFormattedTextExtent(text, draw_area, Justified, x_scale) - getTextExtent(text, x_scale);

			// The number of spaces and tabs in the current line
			unsigned int space_count = 0;
			size_t c;
			for (c = 0; c < char_count; ++c)
				if ((text[c] == ' ') || (text[c] == '\t'))
					++space_count;

			// The width that must be added to each space character in order to transform the left aligned text in justified text
			float shared_lost_space = 0.0;
			if (space_count > 0)
				shared_lost_space = lost_space / (float)space_count;

			for (c = 0; c < char_count; ++c)
			{
				glyph = getGlyphData(text[c]);

				if (glyph)
				{
					const Image* img = glyph->getImage();

					Size size = glyph->getSize(x_scale, y_scale);
					Rect rec (cur_pos.x + glyph->getOffsetX() * x_scale, cur_pos.y + glyph->getOffsetY() * y_scale, cur_pos.x + glyph->getOffsetX() * x_scale + size.width, cur_pos.y + glyph->getOffsetY() * y_scale + size.height);

					m_render.draw(*img, rec, cur_pos.z, clip_rect, colours);

					cur_pos.x += glyph->getAdvance(x_scale);
	#ifdef GUI_KERNING_ENABLED
					if (c < text.length() - 1)
						cur_pos.x += glyph->getKerning(text[c+1]) * x_scale;
	#endif
					// That's where we adjust the size of each space character
					if ((text[c] == ' ') || (text[c] == '\t'))
						cur_pos.x += shared_lost_space;
				}
			}
		}


	}

	float Font::getFormattedTextExtent (const std::string& text, const Rect& format_area, TextFormatting fmt, float x_scale)
	{
		return getFormattedTextExtent(UTF8ToUTF16(text), format_area, fmt, x_scale);
	}

	float Font::getFormattedTextExtent(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale)
	{
		float lineWidth;
		float widest = 0;

		size_t lineStart = 0, lineEnd = 0;
		std::wstring	currLine;

		while (lineEnd < text.length())
		{
			if ((lineEnd = text.find_first_of(L"\n", lineStart)) == std::wstring::npos)
			{
				lineEnd = text.length();
			}

			currLine = text.substr(lineStart, lineEnd - lineStart);
			lineStart = lineEnd + 1;	// +1 to skip \n char

			switch(fmt)
			{
			case Centred:
			case RightAligned:
			case LeftAligned:
				lineWidth = getTextExtent(currLine, x_scale);
				break;

			case Justified:
				// usually we use the width of the rect but we have to ensure the current line is not wider than that
				lineWidth = std::max(format_area.getWidth(), getTextExtent(currLine, x_scale));
				break;

			case WordWrapLeftAligned:
			case WordWrapRightAligned:
			case WordWrapCentred:
				lineWidth = getWrappedTextExtent(currLine, format_area.getWidth(), x_scale);
				break;

			case WordWrapJustified:
				// same as above
				lineWidth = std::max(format_area.getWidth(), getWrappedTextExtent(currLine, format_area.getWidth(), x_scale));
				break;

			default:
				throw std::exception("Font::getFormattedTextExtent - Unknown or unsupported TextFormatting value specified.");
			}

			if (lineWidth > widest)
			{
				widest = lineWidth;
			}

		}

		return widest;
	}

	float Font::getWrappedTextExtent(const std::wstring& text, float wrapWidth, float x_scale)
	{
		std::wstring  whitespace = DefaultWhitespace;
		std::wstring	thisWord;
		size_t	currpos;
		float	lineWidth, wordWidth;
		float	widest = 0;

		// get first word.
		currpos = getNextWord (text, 0, UINT_MAX, thisWord);
		lineWidth = getTextExtent (thisWord, x_scale);

		// while there are words left in the string...
		while (std::wstring::npos != text.find_first_not_of (whitespace, currpos))
		{
			// get next word of the string...
			currpos += getNextWord(text, currpos, UINT_MAX, thisWord);
			wordWidth = getTextExtent (thisWord, x_scale);

			// if the new word would make the string too long
			if ((lineWidth + wordWidth) > wrapWidth)
			{
				if (lineWidth > widest)
					widest = lineWidth;

				// remove whitespace from next word - it will form start of next line
				thisWord = thisWord.substr (thisWord.find_first_not_of (whitespace));
				wordWidth = getTextExtent (thisWord, x_scale);

				// reset for a new line.
				lineWidth = 0;
			}

			// add the next word to the line
			lineWidth += wordWidth;
		}

		if (lineWidth > widest)
			widest = lineWidth;

		return widest;
	}


	void Font::rasterize (utf32 start_codepoint, utf32 end_codepoint)
	{
		// do nothing by default
	}

	TextFormatting StringToFormatType(const std::string& type)
	{
		if(type == "RightAligned") return RightAligned;
		else if(type == "Centred") return Centred;
		else if(type == "Justified") return Justified;
		else if(type == "WordWrapLeftAligned") return WordWrapLeftAligned;
		else if(type == "WordWrapRightAligned") return WordWrapRightAligned;
		else if(type == "WordWrapCentred") return WordWrapCentred;
		else if(type == "WordWrapJustified") return WordWrapJustified;
		return LeftAligned;
	}
}
