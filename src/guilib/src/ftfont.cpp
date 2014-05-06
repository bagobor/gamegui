#include "stdafx.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ftfont.h"

#include "renderer.h"
#include "imagesetmanager.h"

#include "system.h"

// Pixels to put between glyphs
#define INTER_GLYPH_PAD_SPACE 2
// A multiplication coefficient to convert FT_Pos values into normal floats
#define FT_POS_COEF  (1.0/64.0)

namespace
{
	// Font objects usage count
	static int ft_usage_count = 0;
}

// A handle to the FreeType library
static FT_Library ft_lib;

namespace gui
{

FreeTypeFont::FreeTypeFont (const std::string& name, const std::string& filename, unsigned int size, Renderer& r) :
    Font (name, filename, size, r),
    m_antiAliased (true),
    m_fontFace (0)
	//,m_fontData(0),
	//m_fontSize(0)
{
    if (!ft_usage_count++)
        FT_Init_FreeType (&ft_lib);

	updateFont();
}

FreeTypeFont::~FreeTypeFont ()
{
    free ();

    if (!--ft_usage_count)
        FT_Done_FreeType (ft_lib);
}

unsigned int FreeTypeFont::getTextureSize (CodepointMap::const_iterator s, CodepointMap::const_iterator e)
{
    unsigned int texsize = 32; // start with 32x32
    unsigned int max_texsize = m_render.getMaxTextureSize();
    unsigned int glyph_count = 0;

    // Compute approximatively the optimal texture size for font
    while (texsize < max_texsize)
    {
        unsigned int x = INTER_GLYPH_PAD_SPACE, y = INTER_GLYPH_PAD_SPACE;
        unsigned int yb = INTER_GLYPH_PAD_SPACE;
        for (CodepointMap::const_iterator c = s; c != e; ++c)
        {
            // skip glyphs that are already rendered
            if (c->second.getImage ())
                continue;

            // load glyph metrics (don't render)
            if (FT_Load_Char(m_fontFace, c->first,
                             FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT))
                continue;

            unsigned int glyph_w = int (ceil (m_fontFace->glyph->metrics.width * FT_POS_COEF)) +
                INTER_GLYPH_PAD_SPACE;
            unsigned int glyph_h = int (ceil (m_fontFace->glyph->metrics.height * FT_POS_COEF)) +
                INTER_GLYPH_PAD_SPACE;

            x += glyph_w;
            if (x > texsize)
            {
                x = INTER_GLYPH_PAD_SPACE;
                y = yb;
            }
            unsigned int yy = y + glyph_h;
            if (yy > texsize)
                goto too_small;

            if (yy > yb)
                yb = yy;

            ++glyph_count;
        }
        // Okay, the texture size is enough for holding our glyphs
        break;

too_small:
        texsize *= 2;
    }

    return glyph_count ? texsize : 0;
}

void FreeTypeFont::rasterize (utf32 start_codepoint, utf32 end_codepoint)
{
    CodepointMap::iterator s = m_cp_map.lower_bound(start_codepoint);
    if (s == m_cp_map.end ())
        return;

    CodepointMap::iterator orig_s = s;
    CodepointMap::iterator e = m_cp_map.upper_bound (end_codepoint);
    while (true)
    {
        // Create a new Imageset for glyphs
        unsigned int texsize = getTextureSize (s, e);
        // If all glyphs were already rendered, do nothing
        if (!texsize)
            break;

		TexturePtr p = m_render.createTexture(texsize, texsize, Texture::PF_RGBA);
		Imageset *is = new Imageset("glyphimgset");
		size_t ordinal = is->AppendTexture(p);
        m_glyphImages.push_back(is);

        // Create a memory buffer where we will render our glyphs
        argb_t *mem_buffer = new argb_t[texsize * texsize];
        memset (mem_buffer, 0, texsize * texsize * sizeof (argb_t));

        // Go ahead, line by line, top-left to bottom-right
        unsigned int x = INTER_GLYPH_PAD_SPACE, y = INTER_GLYPH_PAD_SPACE;
        unsigned int yb = INTER_GLYPH_PAD_SPACE;

        // Set to true when we finish rendering all glyphs we were asked to
        bool finished = false;
        // Set to false when we reach m_cp_map.end() and we start going backward
        bool forward = true;

        /* To conserve texture space we will render more glyphs than asked,
         * but never less than asked. First we render all glyphs from s to e
         * and after that we render glyphs until we reach m_cp_map.end(),
         * and if there's still free texture space we will go backward
         * from s until we hit m_cp_map.begin().
         */
        while (s != m_cp_map.end())
        {
            // Check if we finished rendering all the required glyphs
            finished |= (s == e);

			FontGlyph &cur_glyph = s->second;

            // Check if glyph already rendered
            if (!s->second.getImage())
            {
                // Render the glyph
                if (FT_Load_Char (m_fontFace, s->first, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT |
                                  (m_antiAliased ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO)))
                {
                    // Create a 'null' image for this glyph so we do not seg later
                    Rect area(0, 0, 0, 0);
                    point offset(0, 0);
                    std::string name;
                    name += s->first;
					s->second.setOffsetX(offset.x);
					s->second.setOffsetY(offset.y);

					SubImage info;
					info.m_ordinal = ordinal;
					info.m_src = area;
					Image::SubImages data;
					data.push_back(info);
					is->DefineImage(name, area.getSize(), data);
                    cur_glyph.setImage(is->GetImage(name));
                }
                else
                {
                    unsigned int glyph_w = m_fontFace->glyph->bitmap.width + INTER_GLYPH_PAD_SPACE;
                    unsigned int glyph_h = m_fontFace->glyph->bitmap.rows + INTER_GLYPH_PAD_SPACE;

                    // Check if glyph right margin does not exceed texture size
                    unsigned int x_next = x + glyph_w;
                    if (x_next > texsize)
                    {
                        x = INTER_GLYPH_PAD_SPACE;
                        x_next = x + glyph_w;
                        y = yb;
                    }

                    // Check if glyph bottom margine does not exceed texture size
                    unsigned int y_bot = y + glyph_h;
                    if (y_bot > texsize)
                        break;

                    // Copy rendered glyph to memory buffer in RGBA format
                    drawGlyphToBuffer (mem_buffer + (y * texsize) + x, texsize);

                    // Create a new image in the imageset

                    point offset (m_fontFace->glyph->metrics.horiBearingX * float(FT_POS_COEF),
                                  -m_fontFace->glyph->metrics.horiBearingY * float(FT_POS_COEF));

                    std::string name;
                    name += s->first;
					cur_glyph.setOffsetX(offset.x);
					cur_glyph.setOffsetY(offset.y);

					{
						float width = (float)(glyph_w - INTER_GLYPH_PAD_SPACE);
						float height = (float)(glyph_h - INTER_GLYPH_PAD_SPACE);

	                    Rect area (float(x), float(y), float(x + width), float(y + height));

						SubImage info;
						info.m_ordinal = ordinal;
						info.m_src = area;
						Image::SubImages data;
						data.push_back(info);
						is->DefineImage(name, area.getSize(), data);
						const Image* img = is->GetImage(name);
						assert(img);
						cur_glyph.setImage(img);
						cur_glyph.SetSize(width, height);
					}
                    
                    // Advance to next position
                    x = x_next;
                    if (y_bot > yb)
                    {
                        yb = y_bot;
                    }
                }
            }

            // Go to next glyph, if we are going forward
            if (forward)
                if (++s == m_cp_map.end ())
                {
                    finished = true;
                    forward = false;
                    s = orig_s;
                }
            // Go to previous glyph, if we are going backward
            if (!forward)
                if (--s == m_cp_map.begin ())
                    break;
        }

        // Copy our memory buffer into the texture and free it
		m_render.updateTexture(is->GetTexture(ordinal), mem_buffer, texsize, texsize, Texture::PF_RGBA);
        delete [] mem_buffer;

        if (finished)
            break;
    }
}

void FreeTypeFont::drawGlyphToBuffer (argb_t *buffer, unsigned int buf_width)
{
    FT_Bitmap *glyph_bitmap = &m_fontFace->glyph->bitmap;

    for (int i = 0; i < glyph_bitmap->rows; ++i)
    {
        unsigned char *src = glyph_bitmap->buffer + (i * glyph_bitmap->pitch);
        switch (glyph_bitmap->pixel_mode)
        {
            case FT_PIXEL_MODE_GRAY:
                {
                    unsigned char *dst = reinterpret_cast<unsigned char*> (buffer);
                    for (int j = 0; j < glyph_bitmap->width; ++j)
                    {
                        // RGBA
                        *dst++ = 0xFF;
                        *dst++ = 0xFF;
                        *dst++ = 0xFF;
                        *dst++ = *src++;
                    }
                }
                break;

            case FT_PIXEL_MODE_MONO:
                for (int j = 0; j < glyph_bitmap->width; ++j)
                    buffer [j] = (src [j / 8] & (0x80 >> (j & 7))) ? 0xFFFFFFFF : 0x00000000;
                break;

            default:
				throw std::exception("Font::drawGlyphToBuffer - The glyph could not be drawn because the pixel mode is unsupported.");
                break;
        }

        buffer += buf_width;
    }
}

void FreeTypeFont::free ()
{
    if (!m_fontFace)
        return;

	m_cp_map_index.clear();
    m_cp_map.clear();

	ImagesetVector::iterator i = m_glyphImages.begin();
	ImagesetVector::iterator end = m_glyphImages.end();
	while(i!=end)
	{
		delete (*i);
		++i;
	}		
    m_glyphImages.clear();

    FT_Done_Face (m_fontFace);
    m_fontFace = 0;
	
	m_font_data.reset();	
}


void FreeTypeFont::updateFont()
{
    free ();

	if(!m_fileName.empty())
	{
		m_font_data = m_render.filesystem()->load_binary(m_fileName);
	}

	if (!m_font_data) return;

    // create face using input font
    if (FT_New_Memory_Face (ft_lib, (const FT_Byte*)m_font_data->ptr, m_font_data->size, 0, &m_fontFace) != 0)
		throw std::exception("FreeTypeFont::load - The source font file does not contain a valid FreeType font.");

    // check that default Unicode character map is available
    if (!m_fontFace->charmap)
    {
        FT_Done_Face (m_fontFace);
        m_fontFace = 0;
        throw std::exception("FreeTypeFont::load - The font does not have a Unicode charmap, and cannot be used.");
    }

    unsigned int horzdpi = m_render.getHorzScreenDPI ();
    unsigned int vertdpi = m_render.getVertScreenDPI ();

    float hps = (float)(m_size * 64);
    float vps = (float)(m_size * 64);

    if (FT_Set_Char_Size (m_fontFace, FT_F26Dot6 (hps), FT_F26Dot6 (vps), horzdpi, vertdpi))
    {
        // For bitmap fonts we can render only at specific point sizes.
        // Try to find nearest point size and use it, if that is possible
        float ptSize_72 = (m_size * 72.0f) / vertdpi;
        float best_delta = 99999;
        float best_size = 0;
        for (int i = 0; i < m_fontFace->num_fixed_sizes; i++)
        {
            float size = m_fontFace->available_sizes [i].size * float(FT_POS_COEF);
            float delta = fabs (size - ptSize_72);
            if (delta < best_delta)
            {
                best_delta = delta;
                best_size = size;
            }
        }

        if ((best_size <= 0) ||
            FT_Set_Char_Size (m_fontFace, 0, FT_F26Dot6 (best_size * 64), 0, 0))
        {
			throw std::exception("FreeTypeFont::load - The font cannot be rasterized at a size of points, and cannot be used.");
        }
    }

    if (m_fontFace->face_flags & FT_FACE_FLAG_SCALABLE)
    {
        //float x_scale = m_fontFace->size->metrics.x_scale * FT_POS_COEF * (1.0/65536.0);
        float y_scale = m_fontFace->size->metrics.y_scale * float(FT_POS_COEF) * (1.0f/65536.0f);
        m_ascender = m_fontFace->ascender * y_scale;
        m_descender = m_fontFace->descender * y_scale;
        m_height = m_fontFace->height * y_scale;
    }
    else
    {
        m_ascender = m_fontFace->size->metrics.ascender * float(FT_POS_COEF);
        m_descender = m_fontFace->size->metrics.descender * float(FT_POS_COEF);
        m_height = m_fontFace->size->metrics.height * float(FT_POS_COEF);
    }

    // Create an empty FontGlyph structure for every glyph of the font
    FT_UInt gindex;
    FT_ULong codepoint = FT_Get_First_Char (m_fontFace, &gindex);
    FT_ULong max_codepoint = codepoint;
    while (gindex)
    {
        if (max_codepoint < codepoint)
            max_codepoint = codepoint;

        // load-up required glyph metrics (don't render)
        if (FT_Load_Char(m_fontFace, codepoint,
                         FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT))
            continue; // glyph error

        float adv = std::max<float>(int(m_fontFace->glyph->metrics.horiAdvance * float(FT_POS_COEF)), 2.0f);

        // create a new empty FontGlyph with given character code
        //m_cp_map[codepoint] = FontGlyph (adv);
		CodepointMap::iterator glyph_it = m_cp_map.insert(CodepointMap::value_type(codepoint, FontGlyph(adv))).first;
		m_cp_map_index.insert(std::make_pair(codepoint, glyph_it));
#ifdef GUI_KERNING_ENABLED
		if (FT_HAS_KERNING(m_fontFace))
		{
			FT_UInt  right;
			FT_ULong right_code = FT_Get_First_Char(m_fontFace, &right);
			for (;right; right_code = FT_Get_Next_Char(m_fontFace, right_code, &right))
			{
				FT_Vector kern_vect;
				FT_Get_Kerning(m_fontFace, gindex, right, FT_KERNING_DEFAULT, &kern_vect);
				if (!kern_vect.x)
					continue;
				(*glyph_it).second.addKerning(right_code, kern_vect.x * float(FT_POS_COEF));
			}
		}
#endif

        // proceed to next glyph
        codepoint = FT_Get_Next_Char (m_fontFace, codepoint, &gindex);
    }

    setMaxCodepoint (max_codepoint);
}

}