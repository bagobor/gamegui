#include "stdafx.h"
#include "renderer.h"

#include "imagesetmanager.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ftfont.h"

enum
{
	VERTEX_PER_QUAD = 4,
	VERTEX_PER_TRIANGLE = 3,
	QUADS_BUFFER = 8000,
	VERTEXBUFFER_CAPACITY = QUADS_BUFFER * VERTEX_PER_QUAD,
	INDEXBUFFER_CAPACITY = QUADS_BUFFER * 6,
};

namespace gui
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355)
#endif

Renderer::Renderer(RenderDevice& render_device, filesystem_ptr fs) :
	GuiZInitialValue(1.0f),
	GuiZElementStep(0.001f),
	GuiZLayerStep(0.0001f),
	m_isQueueing(true),
	m_hdpi(96),
	m_vdpi(96),
	m_maxTextureSize(4096),
	m_originalsize(1.f, 1.f),
	m_texmanager(*this),
	m_autoScale(false),
	m_num_quads(0),
	m_num_batches(0),
	m_currentCapturing(0),
	m_filesystem(fs),
	m_render_device(render_device)
{
	assert (fs && "Filesystem must be provided!");

	m_quads.resize(10000);
	m_batches.resize(1000);
	resetZValue();
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

Renderer::~Renderer(void)
{

}

void Renderer::addCallback(AfterRenderCallbackFunc callback, base_window* window, const Rect& dest, const Rect& clip)
{
	// если сразу должны были рисовать, то сразу запускаем коллбак
	if (!m_isQueueing)
	{
		if (window && callback)
			callback(window, dest, clip);
		return;
	}

	m_needToAddCallback = true;
	m_callbackInfo.afterRenderCallback = callback;
	m_callbackInfo.window = window;
	m_callbackInfo.dest = dest;
	m_callbackInfo.clip = clip;
}



FontPtr	Renderer::createFont(const std::string& name, const std::string& fontname, unsigned int size) {
	return FontPtr(new FreeTypeFont(name, fontname, size, *this));
}

void Renderer::doRender() {
	Size scale (1.f, 1.f);
	/*if (m_autoScale)
	{
		Size& viewport = getViewportSize();
		scale.width = viewport.width / m_originalsize.width;
		scale.height = viewport.height / m_originalsize.height;
	}*/

	m_render_device.render(m_batches, m_quads, m_num_batches, scale);
}

void Renderer::immediateDraw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect,const ColorRect& colors)
{
	// get the rect area that we will actually draw to (i.e. perform clipping)
	Rect final_rect(dest_rect.getIntersection(clip_rect));

	// check if rect was totally clipped
	if (final_rect.getWidth() != 0)
	{
		size_t images = img.GetCount();
		for(size_t i = 0; i < images; ++i)
		{
			RenderImageInfo info;
			img.GetRenderInfo(info, i);
			if(!info.texture)
				continue;
			const Rect& source_rect = info.pixel_rect;

			const float x_scale = 1.f;
			const float y_scale = 1.f;

			float tex_per_pix_x = source_rect.getWidth() / dest_rect.getWidth();
			float tex_per_pix_y = source_rect.getHeight() / dest_rect.getHeight();

			// calculate final, clipped, texture co-ordinates
			Rect  tex_rect((source_rect.m_left + ((final_rect.m_left - dest_rect.m_left) * tex_per_pix_x)) * x_scale,
				(source_rect.m_top + ((final_rect.m_top - dest_rect.m_top) * tex_per_pix_y)) * y_scale,
				(source_rect.m_right + ((final_rect.m_right - dest_rect.m_right) * tex_per_pix_x)) * x_scale,
				(source_rect.m_bottom + ((final_rect.m_bottom - dest_rect.m_bottom) * tex_per_pix_y)) * y_scale);

			tex_rect *= info.texture->getSize();

			// queue a quad to be rendered
			QuadInfo quad;
			Rect rc(final_rect);
			rc.m_left += info.offset.x;
			rc.m_top += info.offset.y;
			fillQuad(quad, rc, tex_rect, z, info, colors);
			m_render_device.renderImmediate(quad);
		}
	}
}

void Renderer::draw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect,const ColorRect& colors, ImageOps horz, ImageOps vert)
{
	Size imgSz = img.GetSize();
    
	unsigned int horzTiles = 1;
	unsigned int vertTiles = 1;

	float xpos = dest_rect.m_left;
	float ypos = dest_rect.m_top;

	switch (horz)
    {		
	case Tile:
		if(dest_rect.getWidth() <= 0.f)
			return;
		horzTiles = (unsigned int)((dest_rect.getWidth() + (imgSz.width - 1)) / imgSz.width);
		break;
	case Stretch:
	default:
		imgSz.width = dest_rect.getWidth();
		break;
	}

	switch (vert)
    {
	case Tile:
		if(dest_rect.getHeight() <= 0.f)
			return;
		vertTiles = (unsigned int)((dest_rect.getHeight() + (imgSz.height - 1)) / imgSz.height);
		break;
	case Stretch:
	default:
		imgSz.height = dest_rect.getHeight();
		break;
	}

	// perform final rendering (actually is now a caching of the images which will be drawn)
    Rect finalRect;
    Rect finalClipper(clip_rect.getIntersection(dest_rect));
    finalRect.m_top = ypos;
    finalRect.m_bottom = ypos + imgSz.height;

    for (unsigned int row = 0; row < vertTiles; ++row)
    {
        finalRect.m_left = xpos;
        finalRect.m_right = xpos + imgSz.width;

        for (unsigned int col = 0; col < horzTiles; ++col)
        {
            // add image to the rendering cache for the target window.
            draw(img, finalRect, z, finalClipper, colors/*,  quad_split_mode*/);

            finalRect.m_left += imgSz.width;
            finalRect.m_right += imgSz.width;
        }

        finalRect.m_top += imgSz.height;
        finalRect.m_bottom += imgSz.height;
    }
}

void Renderer::drawLine(const Image& img, const vec2* p, size_t size, float z, const Rect& clip_rect, const Color& color, float width)
{
	ColorRect color_rect(color);

	vec2 p0, p1, p2, p3;

	size_t images = img.GetCount();
	for(size_t i = 0; i < images; ++i)
	{
		RenderImageInfo info;
		img.GetRenderInfo(info, i);
		if(!info.texture)
			continue;

		Rect source_rect = info.pixel_rect;
		source_rect *= info.texture->getSize();

		for(size_t i = 0; i < size - 1; ++i)
		{
			const vec2& v1 = p[i];
			const vec2& v2 = p[i+1];

			vec2 dir = make_normal(v2-v1);

			vec2 nv(-dir.y, dir.x);
			nv *= width*0.5f;

			p0 = v1 + nv;
			p1 = v2 + nv;

			p2 = v1 - nv;
			p3 = v2 - nv;

			p0.x += info.offset.x;
			p2.x += info.offset.x;
			p0.y += info.offset.y;
			p1.y += info.offset.y;

			addQuad(p0, p1, p2, p3, source_rect, z, info, color_rect);
		}
	}
}

void Renderer::draw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect,const ColorRect& colors)
{
	const Size& sz = img.GetSize();
	size_t images = img.GetCount();
	for(size_t i = 0; i < images; ++i)
	{
		RenderImageInfo info;
		img.GetRenderInfo(info, i);
		if(!info.texture)
			continue;

		float orig_scale_x = dest_rect.getWidth() / sz.width;
		float orig_scale_y = dest_rect.getHeight() / sz.height;

		Rect sub(info.offset + info.crop, info.pixel_rect.getSize());

		Rect dest(
			sub.m_left * orig_scale_x,
			sub.m_top * orig_scale_y,
			sub.m_right * orig_scale_x,
			sub.m_bottom * orig_scale_y);
		dest.offset(dest_rect.getPosition());

		// get the rect area that we will actually draw to (i.e. perform clipping)
		Rect final_rect(dest.getIntersection(clip_rect));
		if (final_rect.empty())
			continue;

		const Rect& source_rect = info.pixel_rect;

		float tex_per_pix_x = source_rect.getWidth() / dest.getWidth();
		float tex_per_pix_y = source_rect.getHeight() / dest.getHeight();

		// calculate final, clipped, texture co-ordinates
		Rect  tex_rect(
			(source_rect.m_left + ((final_rect.m_left - dest.m_left) * tex_per_pix_x)),
			(source_rect.m_top + ((final_rect.m_top - dest.m_top) * tex_per_pix_y)),
			(source_rect.m_right + ((final_rect.m_right - dest.m_right) * tex_per_pix_x)),
			(source_rect.m_bottom + ((final_rect.m_bottom - dest.m_bottom) * tex_per_pix_y))
			);
		
		tex_rect *= info.texture->getSize();

		// queue a quad to be rendered
		addQuad(final_rect, tex_rect, z, info, colors);
	}
}

void Renderer::drawFromCache(base_window* window)
{
	assert(window);
	QuadCacheMap::iterator i = m_mapQuadList.find(window);
	assert(i != m_mapQuadList.end());
	QuadCacheRecord& v = i->second;

	assert(v.num <= v.m_vec.size());

	QuadInfo* cached_quads = &v.m_vec.front();

	if (m_num_quads + v.num >= m_quads.size())
		m_quads.resize((m_num_quads + v.num) * 2);

	BatchInfo* batches = &m_batches.front();
	QuadInfo* quads = &m_quads.front();

	for (std::size_t a = 0; a < v.num; ++a)
	{
		quads[m_num_quads] = cached_quads[a];

		if (!m_num_quads || quads[m_num_quads - 1].texture != quads[m_num_quads].texture ||
			m_needToAddCallback ||
			(m_num_batches && (m_num_quads - batches[m_num_batches - 1].startQuad + 1)*VERTEX_PER_QUAD >= VERTEXBUFFER_CAPACITY))
		{
			// terminate current batch if one:
			if (m_num_batches)
			{
				batches[m_num_batches - 1].numQuads = m_num_quads - batches[m_num_batches - 1].startQuad;
				if (!m_needToAddCallback)
				{
					m_callbackInfo.window = nullptr;
					m_callbackInfo.afterRenderCallback = nullptr;
				}
				m_needToAddCallback = false;
				batches[m_num_batches - 1].callbackInfo = m_callbackInfo;
			}

			// start next batch:
			batches[m_num_batches].texture = quads[m_num_quads].texture;
			batches[m_num_batches].startQuad = m_num_quads;

			++m_num_batches;
		}

		++m_num_quads;
	}
}

void Renderer::clearCache(base_window* window)
{
	if (window)
	{
		QuadCacheMap::iterator i = m_mapQuadList.find(window);
		if (i != m_mapQuadList.end())
			i->second.num = 0;//erase(window);
		//m_mapQuadList[window].m_vec.resize(0);
	}
	else
		m_mapQuadList.clear();
}

bool Renderer::isExistInCache(base_window* window) const
{
	QuadCacheMap::const_iterator i = m_mapQuadList.find(window);
	return i != m_mapQuadList.end() && i->second.num > 0;
}

void Renderer::startCaptureForCache(base_window* window)
{
	QuadCacheMap::iterator i = m_mapQuadList.find(window);
	if (i == m_mapQuadList.end())
	{
		QuadCacheRecord& v = m_mapQuadList[window];
		v.m_vec.resize(100);
		v.num = 0;
		m_currentCapturing = &v;
	}
	else
	{		
		m_currentCapturing = &(i->second);
		m_currentCapturing->num = 0;
	}
		
}
void Renderer::endCaptureForCache(base_window* window)
{
	m_currentCapturing = NULL;
}


void Renderer::clearRenderList(void)
{
	//m_quadlist.clear();
	m_num_quads = 0;
	m_num_batches = 0;
}

void Renderer::beginBatching()
{
	m_needToAddCallback = false;
	clearRenderList();
}

void Renderer::endBatching()
{ 
	if (!m_num_batches) return;
	// закончим последний батч
	if (!m_num_batches) return;
	m_batches[m_num_batches - 1].numQuads = m_num_quads - m_batches[m_num_batches - 1].startQuad;

	if (!m_needToAddCallback)
	{
		m_callbackInfo.window = nullptr;
		m_callbackInfo.afterRenderCallback = nullptr;
	}
	m_needToAddCallback = false;
	m_batches[m_num_batches - 1].callbackInfo = m_callbackInfo;
}


void Renderer::sortQuads(void)
{
}

const Size Renderer::getSize(void)
{
	if(m_autoScale)
		return getOriginalSize();
	else
		return getViewportSize();
}

void Renderer::cleanup(bool complete)
{
	m_num_quads = 0;
	m_num_batches = 0;
	clearCache();
	if(complete)
	{
		m_texmanager.cleanup();
	}	
}

void Renderer::computeVirtualDivRealFactor(Size& coefOut) const
{
	if (!m_autoScale)
	{
		coefOut.width = 1.0f;
		coefOut.height = 1.0f;
		return;
	}
	const Size& original = getOriginalSize(); 
	const Size& current = getViewportSize();
	coefOut = current/original;	
}

Rect Renderer::virtualToRealCoord( const Rect& virtualRect ) const
{
	Rect result = virtualRect;
	Size k;
	computeVirtualDivRealFactor(k);
	result.m_left *= k.width;
	result.m_right *= k.width;
	result.m_top *= k.height;
	result.m_bottom *= k.height;
	return result;
}

Rect Renderer::realToVirtualCoord( const Rect& realRect ) const
{
	Size k;
	computeVirtualDivRealFactor(k);
	Rect result = realRect;
	result.m_left /= k.width;
	result.m_right /= k.width;
	result.m_top /= k.height;
	result.m_bottom /= k.height;
	return result;
}

void Renderer::fillQuad(QuadInfo& quad, const Rect& rc, const Rect& uv, float z, const RenderImageInfo& img, const ColorRect& colors)
{
	quad.positions[0].x	= quad.positions[2].x = rc.m_left;
	quad.positions[0].y	= quad.positions[1].y = rc.m_top;
	quad.positions[1].x	= quad.positions[3].x = rc.m_right;
	quad.positions[2].y	= quad.positions[3].y = rc.m_bottom;

	assert(img.texture);
	quad.z				= z;
	quad.texture		= img.texture;
	quad.texPosition	= uv;
	quad.topLeftCol		= colors.m_top_left.getARGB();
	quad.topRightCol	= colors.m_top_right.getARGB();
	quad.bottomLeftCol	= colors.m_bottom_left.getARGB();
	quad.bottomRightCol	= colors.m_bottom_right.getARGB();
	quad.isAdditiveBlend = img.isAdditiveBlend;
}

void Renderer::addQuad(const vec2& p0, const vec2& p1, const vec2& p2, const vec2& p3, const Rect& tex_rect, float z, const RenderImageInfo& img, const ColorRect& colors) {
	if (m_num_quads >= m_quads.size())
	{
		m_quads.resize(m_num_quads * 2);
	}

	QuadInfo* quads = &m_quads.front();

	QuadInfo& quad = quads[m_num_quads];

	quad.positions[0].x = p0.x;
	quad.positions[0].y = p0.y;

	quad.positions[1].x = p1.x;
	quad.positions[1].y = p1.y;

	quad.positions[2].x = p2.x;
	quad.positions[2].y = p2.y;

	quad.positions[3].x = p3.x;
	quad.positions[3].y = p3.y;

	quad.z = z;
	quad.texture = img.texture;
	quad.texPosition = tex_rect;
	quad.topLeftCol = colors.m_top_left.getARGB();
	quad.topRightCol = colors.m_top_right.getARGB();
	quad.bottomLeftCol = colors.m_bottom_left.getARGB();
	quad.bottomRightCol = colors.m_bottom_right.getARGB();

	// if not queering, render directly (as in, right now!)
	if (!m_isQueueing)
	{
		m_render_device.renderImmediate(quad);
		return;
	}

	if (m_currentCapturing)
	{
		if (m_currentCapturing->num >= m_currentCapturing->m_vec.size())
			m_currentCapturing->m_vec.resize(m_currentCapturing->num * 2);

		QuadInfo& q = (&m_currentCapturing->m_vec.front())[m_currentCapturing->num];
		q = quad;
		++(m_currentCapturing->num);
	}

	BatchInfo* batches = &m_batches[0];


	if (!m_num_quads || quads[m_num_quads - 1].texture != quad.texture ||
		m_needToAddCallback ||
		(m_num_batches && (m_num_quads - batches[m_num_batches - 1].startQuad + 1)*VERTEX_PER_QUAD >= VERTEXBUFFER_CAPACITY))
	{
		// finalize prev batch if one
		if (m_num_batches)
		{
			batches[m_num_batches - 1].numQuads = m_num_quads - batches[m_num_batches - 1].startQuad;

			if (!m_needToAddCallback)
			{
				m_callbackInfo.window = nullptr;
				m_callbackInfo.afterRenderCallback = nullptr;
			}
			m_needToAddCallback = false;
			batches[m_num_batches - 1].callbackInfo = m_callbackInfo;
		}

		// start new batch
		batches[m_num_batches].texture = quad.texture;
		batches[m_num_batches].startQuad = m_num_quads;
		batches[m_num_batches].numQuads = 0;

		++m_num_batches;
	}

	++m_num_quads;
	assert(m_num_batches);
	++batches[m_num_batches - 1].numQuads;
}

}