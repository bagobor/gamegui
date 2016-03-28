#pragma once

#include "rect.h"
#include "colorRect.h"
#include "texture.h"
#include "texmanager.h"

#include "imageops.h"
#include "imagesetmanager.h"

#include <functional>

namespace gui
{
class WindowBase;
typedef std::function <void (WindowBase* wnd, const Rect& dest, const Rect& clip)> AfterRenderCallbackFunc;

struct filesystem;
typedef std::shared_ptr<filesystem> filesystem_ptr;

struct log;

struct RenderImageInfo;

class Font;
typedef std::shared_ptr<Font> FontPtr;

enum OrientationFlags
{
	FlipHorizontal		= 1,	//!< Horizontal flip the image
	FlipVertical		= 2,	//!< Vertical flip the image
	RotateRightAngle	= 4		//!< Rotate the image anticlockwise 90 degree
};

class Image;


struct QuadInfo
{
	struct vec2 { float x, y; };

	// order: 0 ---> 1
	//		  2 ---> 3
	vec2				positions[4];

	float				z;
	Rect				texPosition;
	unsigned long		topLeftCol;
	unsigned long		topRightCol;
	unsigned long		bottomLeftCol;
	unsigned long		bottomRightCol;
	bool				isAdditiveBlend;

	//todo: remove from quad structure
	//Texture*	texture;

	__inline bool operator<(const QuadInfo& other) const
	{
		// this is intentionally reversed.
		return z > other.z;
	}
};

struct RenderCallbackInfo
{
	AfterRenderCallbackFunc afterRenderCallback;
	WindowBase* window;
	Rect dest;
	Rect clip;
};

struct BatchInfo
{
	Texture* texture;
	std::size_t startQuad;
	std::size_t numQuads;
	//QuadInfo* quads;
	bool isAdditiveBlend;

	RenderCallbackInfo callbackInfo;
};

typedef std::vector<QuadInfo> Quads;
typedef std::vector<BatchInfo> Batches;
typedef std::shared_ptr<Texture> TexturePtr;

struct RenderDevice {
	struct ViewPort {
		unsigned x, y, w, h;
	};

	struct RenderCommand {
		TexturePtr texture;
	};

	virtual ~RenderDevice() = 0 {};

	virtual TexturePtr createTexture(const void* data, size_t size, unsigned int width, unsigned int height, Texture::PixelFormat format) = 0;
	virtual TexturePtr createTexture(const std::string& filename) = 0;

	virtual void renderImmediate(const QuadInfo& q, Texture* texture, bool isAdditive = false) = 0;
	virtual void render(const Batches& batches, const Quads& quads, size_t num_batches, Size scale = Size(1.0f, 1.0f)) = 0;

	const ViewPort& getViewport() const { return viewport; }
	virtual void setViewport(const ViewPort& vp) { viewport = vp; }

protected:
	ViewPort viewport;
};

typedef std::shared_ptr<RenderDevice> RenderDevicePtr;

//TODO: rename to Canvas
class  Renderer
{
	friend TextureCache;
	Renderer& operator=(const Renderer&);
public:
	Renderer(RenderDevice& render_device, filesystem_ptr fs);
	virtual ~Renderer();

	void addCallback( AfterRenderCallbackFunc callback,WindowBase* window, const Rect& dest, const Rect& clip);

	void	drawLine(const Image& img, const vec2* p, size_t size, float z, const Rect& clip_rect, const Color& color, float width);
	
	void	draw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect, const ColorRect& colors);
	void	draw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect, const ColorRect& colors, ImageOps horz, ImageOps vert);

	void	immediateDraw(const Image& img, const Rect& dest_rect, float z, const Rect& clip_rect, const ColorRect& colors);
	void	doRender();

	virtual void startCaptureForCache(WindowBase* window);
	virtual void endCaptureForCache(WindowBase* window);
	void drawFromCache(WindowBase* window);
	void clearCache(WindowBase* window = 0);
	
	virtual	void	clearRenderList();	
	
	virtual void	beginBatching();
	virtual void	endBatching();	
	bool			isExistInCache(WindowBase* window) const;
	
	virtual void	setQueueingEnabled(bool queueing)  { m_isQueueing = queueing; }
	bool			isQueueingEnabled(void) const { return m_isQueueing; }

	TexturePtr	createTexture(const std::string& filename);
	TexturePtr	createTexture(const void* data, size_t size, unsigned int width, unsigned int height, Texture::PixelFormat format);
	TexturePtr	createTexture(unsigned int width, unsigned int height, Texture::PixelFormat format);
	TexturePtr	updateTexture(TexturePtr p, const void* data, size_t size, unsigned int width, unsigned int height, Texture::PixelFormat format);

	FontPtr	createFont(const std::string& name, const std::string& fontname, unsigned int size);	

	const Size&	getOriginalSize(void) const	{ return m_originalsize; }
	const Size	getSize(void);
	unsigned int getMaxTextureSize(void) const { return m_maxTextureSize; }
	unsigned int getHorzScreenDPI(void) const { return m_hdpi; }
	unsigned int getVertScreenDPI(void) const { return m_vdpi; }

	void	setOriginalSize(const Size& sz) { m_originalsize = sz; }

	void	resetZValue()				{m_current_z = GuiZInitialValue;}
	void	advanceZValue()				{m_current_z -= GuiZElementStep;}
	float	getCurrentZ() const			{return m_current_z;}
	float	getZLayer(unsigned int layer) const {return m_current_z - ((float)layer * GuiZLayerStep);}

	//bool isAutoScale() const { return m_autoScale; }
	Size getViewportSize() const;
	
	void cleanup(bool complete);

	filesystem_ptr filesystem() {return m_filesystem;}
	RenderDevice& renderDeivce() { return m_render_device; }
	const RenderDevice& renderDeivce() const { return m_render_device; }

protected:
	inline void addQuad(const Rect& r, const Rect& tr, float z, const RenderImageInfo& img, const ColorRect& colours) {
		vec2 p[4];
		p[0].x	= p[2].x = r.m_left;
		p[0].y	= p[1].y = r.m_top;
		p[1].x	= p[3].x = r.m_right;
		p[2].y	= p[3].y = r.m_bottom;

		addQuad(p[0], p[1], p[2], p[3], tr, z, img, colours);
	}

	void addQuad(const vec2& p0, const vec2& p1, const vec2& p2, const vec2& p3, const Rect& tex_rect, float z, const RenderImageInfo& img, const ColorRect& colours);
	void renderQuadDirect(const QuadInfo& q, Texture* texture, bool isAdditive = false) {
		m_render_device.renderImmediate(q, texture, isAdditive);
	}

	void fillQuad(QuadInfo& quad, const Rect& rc, const Rect& uv, float z, const RenderImageInfo& img, const ColorRect& colors);
	void sortQuads();
	
	TexturePtr createTextureInstance(const std::string& filename);

protected:
	TextureCache m_texmanager; 

	Quads m_quads;
	Batches m_batches;

	size_t m_num_quads;
	size_t m_num_batches;
	
	Size m_originalsize;
	unsigned int m_hdpi;
	unsigned int m_vdpi;
	unsigned int m_maxTextureSize;

	bool m_isQueueing;
	//bool m_autoScale;

	const float	GuiZInitialValue;
	const float	GuiZElementStep;
	const float	GuiZLayerStep;
	float	m_current_z;

	//LoggerCallback m_log_cb;
	struct CachedQuad : public QuadInfo {
		Texture* texture;
		bool isAdditive;
	};

	typedef std::vector <CachedQuad> CachedQuadList;
	struct QuadCacheRecord
	{	
		CachedQuadList m_vec;
		std::size_t num;
	};

	typedef std::map <WindowBase*, QuadCacheRecord> QuadCacheMap;
	QuadCacheMap m_mapQuadList;
	QuadCacheRecord* m_currentCapturing;

	filesystem_ptr m_filesystem;
	RenderDevice& m_render_device;

	bool m_needToAddCallback;
	RenderCallbackInfo m_callbackInfo;
};

}