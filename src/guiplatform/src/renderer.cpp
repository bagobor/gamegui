#include "stdafx.h"

#include <guilib/guilib.h>
#include <guilib/src/renderimageinfo.h>

#include "renderer.h"
#include "texture.h"
#include "ftfont.h"

// fine tune :)
#define PixelAligned(x)	( ( (float)(int)(( x ) + (( x ) > 0.0f ? 0.5f : -0.5f)) ) - 0.5f )


namespace gui
{
	namespace ogl_platform
	{
		//Renderer* CreateRenderer(rgde::render::device& dev, filesystem_ptr fs, unsigned buff)
		//{
		//	return new renderer(dev, fs, 1024);
		//}
				
		struct QuadVertex
		{
			float x, y, tu1, tv1;		//!< The transformed position for the vertex.
			unsigned int diffuse;		//!< colour of the vertex
		};

		//using rgde::render::vertex_element;
		//vertex_element vertex_desc[] = 
		//{
		//	{0, 0,  vertex_element::float4,   vertex_element::default_method, vertex_element::position, 0}, 
		//	{0, 16, vertex_element::color4ub, vertex_element::default_method, vertex_element::color,	0},
		//	vertex_element::end_element
		//};

		enum
		{
			VERTEX_PER_QUAD = 4,
			VERTEX_PER_TRIANGLE = 3,
			QUADS_BUFFER = 8000,
			VERTEXBUFFER_CAPACITY = QUADS_BUFFER * VERTEX_PER_QUAD,
			INDEXBUFFER_CAPACITY  = QUADS_BUFFER*6,
		};

		/*************************************************************************
		Constructor
		*************************************************************************/
		RendererGL::RendererGL(filesystem_ptr fs, unsigned int max_quads)
			: Renderer(fs)
		{
			m_needToAddCallback = false;
			Size size(getViewportSize());

			constructor_impl(size);
		}


		/*************************************************************************
		method to do work of constructor
		*************************************************************************/
		void RendererGL::constructor_impl(const Size& display_size)
		{
			//m_bufferPos     = 0;
			m_originalsize = display_size;
			//m_vertexDeclaration = vertex_declaration::create(m_device, vertex_desc, 3);

			// Create a vertex buffer
			//m_buffer = vertex_buffer::create(
			//	m_device, 
			//	m_vertexDeclaration, 
			//	VERTEXBUFFER_CAPACITY * sizeof(QuadVertex),
			//	resource::default, 
			//	buffer::write_only | buffer::dynamic);

			
			//if (!m_buffer)
			//{
				// Ideally, this would have been a RendererException, but we can't use that until the System object is created
				// and that requires a Renderer passed to the constructor, so we throw this instead.
			//	throw std::exception("Creation of VertexBuffer for Renderer object failed");
			//}

			//{
			//	m_ibuffer = index_buffer::create(m_device, INDEXBUFFER_CAPACITY*sizeof(unsigned short), false, resource::default);

			//	unsigned short* data = (unsigned short*)m_ibuffer->lock(0, INDEXBUFFER_CAPACITY*sizeof(unsigned short), 0);

			//	for (int i = 0; i < VERTEXBUFFER_CAPACITY; i += VERTEX_PER_QUAD)
			//	{
			//		const size_t quad_index = (i / VERTEX_PER_QUAD)*6;

			//		data[quad_index+0] = i + 0;
			//		data[quad_index+1] = i + 2;
			//		data[quad_index+2] = i + 1;

			//		data[quad_index+3] = i + 1;
			//		data[quad_index+4] = i + 2;
			//		data[quad_index+5] = i + 3;
			//	}

			//	m_ibuffer->unlock();
			//}

			//if (data_ptr data = m_filesystem->load_binary("shaders\\gui.fx"))
			//{
			//	m_shader = shader_effect::create(m_device, data->ptr, data->size);
			//}			

			// get the maximum available texture size.
			// set max texture size the the smaller of max width and max height.
			m_maxTextureSize = 2048;//devCaps.MaxTextureWidth < devCaps.MaxTextureHeight ? devCaps.MaxTextureWidth : devCaps.MaxTextureHeight;

			//m_handleGuiTexture = m_shader->get_param("guitexture");
			//m_handleViewPortSize = m_shader->get_param("viewportsize");
		}


		/*************************************************************************
		Destructor
		*************************************************************************/
		RendererGL::~RendererGL(void)
		{
		}

		/*************************************************************************
		render a quad directly to the display		
		*************************************************************************/

		namespace 
		{
			// return value = buff offset in QuadInfo
			/*__inline */__forceinline unsigned int fill_vertex(const Renderer::QuadInfo& q, QuadVertex*& v, float scaleX, float scaleY)
			{									
				QuadVertex& v0 = *v; ++v;
				QuadVertex& v1 = *v; ++v;
				QuadVertex& v2 = *v; ++v;
				QuadVertex& v3 = *v; ++v;

				// setup Vertex 1...
				v0.x = PixelAligned(q.positions[0].x * scaleX);
				v1.x = PixelAligned(q.positions[1].x * scaleX);
				v2.x = PixelAligned(q.positions[2].x * scaleX);
				v3.x = PixelAligned(q.positions[3].x * scaleX);

				v0.y = PixelAligned(q.positions[0].y * scaleY);
				v1.y = PixelAligned(q.positions[1].y * scaleY);
				v2.y = PixelAligned(q.positions[2].y * scaleY);
				v3.y = PixelAligned(q.positions[3].y * scaleY);
				
				v0.tu1 = v2.tu1 = q.texPosition.m_left;
				v0.tv1 = v1.tv1 = q.texPosition.m_top;
				v1.tu1 = v3.tu1 = q.texPosition.m_right;
				v2.tv1 = v3.tv1 = q.texPosition.m_bottom;

				v0.diffuse = q.topLeftCol;
				v1.diffuse = q.topRightCol;
				v2.diffuse = q.bottomLeftCol;
				v3.diffuse = q.bottomRightCol;

				return VERTEX_PER_QUAD;
			}			
		}

		/*************************************************************************
		render a quad directly to the display
		*************************************************************************/
		void RendererGL::renderQuadDirect(const QuadInfo& q)
		{
			//if (!m_buffer)
			//	return;
	
			//view_port viewPortDesc;
			//m_device.get_viewport(viewPortDesc);

			//m_device.set_decl(m_vertexDeclaration);

			//m_shader->set_tech("Simple");
			//rgde::math::vec2f vec((float)viewPortDesc.width, (float)viewPortDesc.height);
			//m_shader->set("ViewPortSize",&vec, 2 );
			//m_shader->begin(0 );
			//m_shader->begin_pass(0);

			//texture* tex = (texture*)q.texture;
			//m_device.set_texture(((texture*)tex)->get_platform_resource(), 0 );

			//QuadVertex buffmem[VERTEX_PER_QUAD];

			//float scaleX = 1.f;
			//float scaleY = 1.f;
			//if(m_autoScale)
			//{
			//	const Size viewport = getViewportSize();
			//	scaleX = viewport.width / m_originalsize.width;
			//	scaleY = viewport.height / m_originalsize.height;
			//}

			//QuadVertex* temp_ptr = (QuadVertex*)buffmem;
			//unsigned int vert_filled = fill_vertex(q, temp_ptr, scaleX, scaleY);

			//m_bufferPos = vert_filled;

			//// if bufferPos is 0 there is no data in the buffer and nothing to render
			//if (m_bufferPos == 0)
			//{
			//	return;
			//}

			//typedef rgde::uint16 uint16;

			//const unsigned int prim_count = 2 * m_bufferPos / VERTEX_PER_QUAD;

			//static const uint16 index_data[6] = 
			//{
			//	0,1,2, // 1st triangle
			//	1,2,3  // 2nd triangle
			//};

			//m_device.draw(triangle_list, m_bufferPos, prim_count, buffmem, sizeof(QuadVertex), index_data);			

			//// reset buffer position to 0...
			//m_bufferPos = 0;

			//m_shader->end_pass();
			//m_shader->end();
		}
		
		void RendererGL::setRenderStates()
		{
			// setup vertex stream
			//m_device.set_stream_source(0, m_buffer, sizeof(QuadVertex));
			//m_device.set_index_buffer(m_ibuffer);
			//
			//view_port viewPortDesc;
			//m_device.get_viewport(viewPortDesc);

			//m_device.set_decl(m_vertexDeclaration);

			//m_shader->set_tech("simple");
			//rgde::math::vec2f vec((float)viewPortDesc.width, (float)viewPortDesc.height);

			//if (m_handleViewPortSize)
			//	m_shader->set(m_handleViewPortSize,(float*)&vec, 2 );

			//m_shader->begin(0);
			//m_shader->begin_pass(0);
		}
		/*************************************************************************
		perform final rendering for all queued renderable quads.
		*************************************************************************/
		void RendererGL::doRender(void)
		{
			//if (!m_buffer)
			//	return;

			//setRenderStates();
			//m_currTexture.reset();

			//float scaleX = 1.f;
			//float scaleY = 1.f;
			//if(m_autoScale)
			//{
			//	Size& viewport = getViewportSize();
			//	scaleX = viewport.width / m_originalsize.width;
			//	scaleY = viewport.height / m_originalsize.height;
			//}

			//static DWORD s_quadOffset = 0;	// buffer offset in quads
			//QuadVertex*	buffmem;
			//
			//static const unsigned int quad_size = VERTEX_PER_QUAD * sizeof(QuadVertex);

			//BatchInfo* batches = &m_batches.front();
			//QuadInfo* quads = &m_quads.front();

			//for (std::size_t b = 0; b < m_num_batches; ++b)
			//{
			//	BatchInfo& batch = batches[b];
			//	if ( VERTEX_PER_QUAD * (batch.numQuads + s_quadOffset) >= VERTEXBUFFER_CAPACITY)
			//		s_quadOffset = 0;

			//	buffmem = (QuadVertex*)m_buffer->lock
			//		(
			//		s_quadOffset * quad_size, 
			//		batches[b].numQuads * quad_size,
			//		buffer::nooverwrite
			//		);

			//	if (!buffmem )
			//		break;				

			//	std::size_t numQ = batch.numQuads;
			//	QuadInfo* quad = &quads[batch.startQuad];
			//	for (std::size_t q = 0; q < numQ; ++q, ++quad)
			//	{
			//		fill_vertex(*quad, buffmem, scaleX, scaleY);
			//	}

			//	m_buffer->unlock();

			//	gui::ogl_platform::texture* t = static_cast<gui::ogl_platform::texture*>(batch.texture);
			//	m_device.set_texture(t->get_platform_resource(), 0);

			//	m_device.draw
			//		(
			//		triangle_list,					//type
			//		s_quadOffset * VERTEX_PER_QUAD, // base_vertex_index Ok
			//		0,								// min_vertex_indexOk
			//		numQ*VERTEX_PER_QUAD,			//num_vertices OK
			//		0,								// Ok
			//		numQ*2							// Ok
			//		);

			//	s_quadOffset += (DWORD)numQ;

			//	if (batch.callbackInfo.window && batch.callbackInfo.afterRenderCallback)
			//	{
			//		m_shader->end_pass();
			//		m_shader->end();

			//		batch.callbackInfo.afterRenderCallback(batch.callbackInfo.window, batch.callbackInfo.dest, batch.callbackInfo.clip);

			//		// if it was not last batch
			//		if (b < m_num_batches -1)
			//			setRenderStates();
			//	}
			//}
			//
			//m_shader->end_pass();
			//m_shader->end();
		}

		/*************************************************************************
		setup states etc
		*************************************************************************/
		void RendererGL::initPerFrameStates(void)
		{
			// setup vertex stream
			//m_device.set_stream_source(0, m_buffer, sizeof(QuadVertex));
			//m_device.set_index_buffer(m_ibuffer);
		}

		TexturePtr	RendererGL::updateTexture(TexturePtr p, const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, Texture::PixelFormat pixFormat)
		{
			return TexturePtr();
			//TexturePtr platform_tex = static_cast<texture&>(*p).get_platform_resource();

			//surface_ptr surface = platform_tex->get_surface(0);
			//surface::lock_data ld;
			//bool res = surface->lock(ld);

			//if (!res)
			//{
			//	platform_tex.reset();
			//	throw std::exception("Failed to load texture from memory: IDirect3DTexture9::LockRect failed.");
			//}
			//else
			//{
			//	// copy data from buffer into texture
			//	unsigned long* dst = (unsigned long*)ld.bytes;
			//	unsigned long* src = (unsigned long*)buffPtr;

			//	// RGBA
			//	if (pixFormat == Texture::PF_RGBA)
			//	{
			//		for (unsigned int i = 0; i < buffHeight; ++i)
			//		{
			//			for (unsigned int j = 0; j < buffWidth; ++j)
			//			{
			//				//TODO: check for endian safety on non-MS platforms
			//				unsigned char r = (unsigned char)(src[j] & 0xFF);
			//				unsigned char g = (unsigned char)((src[j] >> 8) & 0xFF);
			//				unsigned char b = (unsigned char)((src[j] >> 16)  & 0xFF);
			//				unsigned char a = (unsigned char)((src[j] >> 24) & 0xFF);

			//				dst[j] = rgde::math::color(r, g, b, a).data;
			//			}

			//			dst += ld.pitch / sizeof(unsigned long);
			//			src += buffWidth;
			//		}
			//	}
			//	// RGB
			//	else
			//	{
			//		for (unsigned int i = 0; i < buffHeight; ++i)
			//		{
			//			for (unsigned int j = 0; j < buffWidth; ++j)
			//			{
			//				dst[j] = src[j];
			//			}

			//			dst += ld.pitch / sizeof(unsigned long);
			//			src += buffWidth;
			//		}
			//	}

			//	surface->unlock();
			//}
			//
			//return p;
		}

	}
}
