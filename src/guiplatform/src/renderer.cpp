#include "stdafx.h"

#include <rgde/render/device.h>
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
		Renderer* CreateRenderer(rgde::render::device& dev, filesystem_ptr fs, unsigned buff)
		{
			return new renderer(dev, fs, 1024);
		}
				
		struct QuadVertex
		{
			float x, y, tu1, tv1;		//!< The transformed position for the vertex.
			unsigned int diffuse;		//!< colour of the vertex
		};

		using rgde::render::vertex_element;
		vertex_element vertex_desc[] = 
		{
			{0, 0,  vertex_element::float4,   vertex_element::default_method, vertex_element::position, 0}, 
			{0, 16, vertex_element::color4ub, vertex_element::default_method, vertex_element::color,	0},
			vertex_element::end_element
		};

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
		renderer::renderer(device& device, filesystem_ptr fs, unsigned int max_quads)
			: Renderer(fs), m_device(device)
		{
			m_needToAddCallback = false;
			Size size(getViewportSize());

			constructor_impl(size);
		}


		/*************************************************************************
		method to do work of constructor
		*************************************************************************/
		void renderer::constructor_impl(const Size& display_size)
		{
			m_bufferPos     = 0;
			m_originalsize = display_size;
			m_vertexDeclaration = vertex_declaration::create(m_device, vertex_desc, 3);

			// Create a vertex buffer
			m_buffer = vertex_buffer::create(
				m_device, 
				m_vertexDeclaration, 
				VERTEXBUFFER_CAPACITY * sizeof(QuadVertex),
				resource::default, 
				buffer::write_only | buffer::dynamic);

			
			if (!m_buffer)
			{
				// Ideally, this would have been a RendererException, but we can't use that until the System object is created
				// and that requires a Renderer passed to the constructor, so we throw this instead.
				throw std::exception("Creation of VertexBuffer for Renderer object failed");
			}

			{
				m_ibuffer = index_buffer::create(m_device, INDEXBUFFER_CAPACITY*sizeof(unsigned short), false, resource::default);

				unsigned short* data = (unsigned short*)m_ibuffer->lock(0, INDEXBUFFER_CAPACITY*sizeof(unsigned short), 0);

				for (int i = 0; i < VERTEXBUFFER_CAPACITY; i += VERTEX_PER_QUAD)
				{
					const size_t quad_index = (i / VERTEX_PER_QUAD)*6;

					data[quad_index+0] = i + 0;
					data[quad_index+1] = i + 2;
					data[quad_index+2] = i + 1;

					data[quad_index+3] = i + 1;
					data[quad_index+4] = i + 2;
					data[quad_index+5] = i + 3;
				}

				m_ibuffer->unlock();
			}

			if (data_ptr data = m_filesystem->load_binary("shaders\\gui.fx"))
			{
				m_shader = shader_effect::create(m_device, data->ptr, data->size);
			}			

			// get the maximum available texture size.
			// set max texture size the the smaller of max width and max height.
			m_maxTextureSize = 2048;//devCaps.MaxTextureWidth < devCaps.MaxTextureHeight ? devCaps.MaxTextureWidth : devCaps.MaxTextureHeight;

			m_handleGuiTexture = m_shader->get_param("guitexture");
			m_handleViewPortSize = m_shader->get_param("viewportsize");
		}


		/*************************************************************************
		Destructor
		*************************************************************************/
		renderer::~renderer(void)
		{
		}

		void renderer::addCallback( AfterRenderCallbackFunc callback,
											base_window* window, const Rect& dest, const Rect& clip)
		{
			// если сразу должны были рисовать, то сразу запускаем коллбак
			if (!m_isQueueing)
			{
				if (window && callback)
					callback(window,dest, clip);
				return;
			}

			m_needToAddCallback = true;
			m_callbackInfo.afterRenderCallback = callback;
			m_callbackInfo.window = window;
			m_callbackInfo.dest = dest;
			m_callbackInfo.clip = clip;
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
		void renderer::renderQuadDirect(const QuadInfo& q)
		{
			if (!m_buffer)
				return;
	
			view_port viewPortDesc;
			m_device.get_viewport(viewPortDesc);

			m_device.set_decl(m_vertexDeclaration);

			m_shader->set_tech("Simple");
			rgde::math::vec2f vec((float)viewPortDesc.width, (float)viewPortDesc.height);
			m_shader->set("ViewPortSize",&vec, 2 );
			m_shader->begin(0 );
			m_shader->begin_pass(0);

			texture* tex = (texture*)q.texture;
			m_device.set_texture(((texture*)tex)->get_platform_resource(), 0 );

			QuadVertex buffmem[VERTEX_PER_QUAD];

			float scaleX = 1.f;
			float scaleY = 1.f;
			if(m_autoScale)
			{
				const Size viewport = getViewportSize();
				scaleX = viewport.width / m_originalsize.width;
				scaleY = viewport.height / m_originalsize.height;
			}

			QuadVertex* temp_ptr = (QuadVertex*)buffmem;
			unsigned int vert_filled = fill_vertex(q, temp_ptr, scaleX, scaleY);

			m_bufferPos = vert_filled;

			// if bufferPos is 0 there is no data in the buffer and nothing to render
			if (m_bufferPos == 0)
			{
				return;
			}

			typedef rgde::uint16 uint16;

			const unsigned int prim_count = 2 * m_bufferPos / VERTEX_PER_QUAD;

			static const uint16 index_data[6] = 
			{
				0,1,2, // 1st triangle
				1,2,3  // 2nd triangle
			};

			m_device.draw(triangle_list, m_bufferPos, prim_count, buffmem, sizeof(QuadVertex), index_data);			

			// reset buffer position to 0...
			m_bufferPos = 0;

			m_shader->end_pass();
			m_shader->end();
		}

		void renderer::addQuad(const vec2& p0, const vec2& p1, const vec2& p2, const vec2& p3, const Rect& tex_rect, float z, const RenderImageInfo& img, const ColorRect& colors)
		{
			if (m_num_quads >= m_quads.size())
			{
				m_quads.resize(m_num_quads*2);
			}

			QuadInfo* quads = &m_quads.front();

			QuadInfo& quad = quads[m_num_quads];

			quad.positions[0].x	= p0.x;
			quad.positions[0].y	= p0.y;

			quad.positions[1].x	= p1.x;
			quad.positions[1].y	= p1.y;

			quad.positions[2].x	= p2.x;
			quad.positions[2].y	= p2.y;

			quad.positions[3].x	= p3.x;
			quad.positions[3].y	= p3.y;

			quad.z				= z;
			quad.texture		= img.texture;
			quad.texPosition	= tex_rect;
			quad.topLeftCol		= colors.m_top_left.getARGB();
			quad.topRightCol	= colors.m_top_right.getARGB();
			quad.bottomLeftCol	= colors.m_bottom_left.getARGB();
			quad.bottomRightCol	= colors.m_bottom_right.getARGB();

			// if not queering, render directly (as in, right now!)
			if (!m_isQueueing)
			{
				renderQuadDirect(quad);
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


			if (!m_num_quads  || quads[m_num_quads - 1].texture != quad.texture ||
				m_needToAddCallback || 
				(m_num_batches && (m_num_quads - batches[m_num_batches - 1].startQuad + 1)*VERTEX_PER_QUAD >= VERTEXBUFFER_CAPACITY))
			{
				// finalize prev batch if one
				if (m_num_batches)
				{
					batches[m_num_batches - 1].numQuads = m_num_quads - batches[m_num_batches - 1].startQuad;

					if (!m_needToAddCallback)
					{
						m_callbackInfo.window = NULL;
						m_callbackInfo.afterRenderCallback = NULL;
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

		void renderer::addQuad(const Rect& dest_rect, const Rect& tex_rect, float z, const RenderImageInfo& img, const ColorRect& colors)		
		{
			if (m_num_quads >= m_quads.size())
			{
				m_quads.resize(m_num_quads*2);
			}

			QuadInfo* quads = &m_quads.front();
			QuadInfo& quad = quads[m_num_quads];

			fillQuad(quad, dest_rect, tex_rect, z, img, colors);
		
			// if not queering, render directly (as in, right now!)
			if (!m_isQueueing)
			{
				renderQuadDirect(quad);
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

			if (!m_num_quads  || quads[m_num_quads - 1].texture != quad.texture ||
				m_needToAddCallback || 
				(m_num_batches && (m_num_quads - batches[m_num_batches - 1].startQuad + 1)*VERTEX_PER_QUAD >= VERTEXBUFFER_CAPACITY))
			{
				// finalize prev batch if one
				if (m_num_batches)
				{
					batches[m_num_batches - 1].numQuads = m_num_quads - batches[m_num_batches - 1].startQuad;
					
					if (!m_needToAddCallback)
					{
						m_callbackInfo.window = NULL;
						m_callbackInfo.afterRenderCallback = NULL;
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

		void renderer::setRenderStates()
		{
			// setup vertex stream
			m_device.set_stream_source(0, m_buffer, sizeof(QuadVertex));
			m_device.set_index_buffer(m_ibuffer);
			
			view_port viewPortDesc;
			m_device.get_viewport(viewPortDesc);

			m_device.set_decl(m_vertexDeclaration);

			m_shader->set_tech("simple");
			rgde::math::vec2f vec((float)viewPortDesc.width, (float)viewPortDesc.height);

			if (m_handleViewPortSize)
				m_shader->set(m_handleViewPortSize,(float*)&vec, 2 );

			m_shader->begin(0);
			m_shader->begin_pass(0);
		}
		/*************************************************************************
		perform final rendering for all queued renderable quads.
		*************************************************************************/
		void renderer::doRender(void)
		{
			if (!m_buffer)
				return;

			setRenderStates();
			m_currTexture.reset();

			float scaleX = 1.f;
			float scaleY = 1.f;
			if(m_autoScale)
			{
				Size& viewport = getViewportSize();
				scaleX = viewport.width / m_originalsize.width;
				scaleY = viewport.height / m_originalsize.height;
			}

			static DWORD s_quadOffset = 0;	// buffer offset in quads
			QuadVertex*	buffmem;
			
			static const unsigned int quad_size = VERTEX_PER_QUAD * sizeof(QuadVertex);

			BatchInfo* batches = &m_batches.front();
			QuadInfo* quads = &m_quads.front();

			for (std::size_t b = 0; b < m_num_batches; ++b)
			{
				BatchInfo& batch = batches[b];
				if ( VERTEX_PER_QUAD * (batch.numQuads + s_quadOffset) >= VERTEXBUFFER_CAPACITY)
					s_quadOffset = 0;

				buffmem = (QuadVertex*)m_buffer->lock
					(
					s_quadOffset * quad_size, 
					batches[b].numQuads * quad_size,
					buffer::nooverwrite
					);

				if (!buffmem )
					break;				

				std::size_t numQ = batch.numQuads;
				QuadInfo* quad = &quads[batch.startQuad];
				for (std::size_t q = 0; q < numQ; ++q, ++quad)
				{
					fill_vertex(*quad, buffmem, scaleX, scaleY);
				}

				m_buffer->unlock();

				gui::ogl_platform::texture* t = static_cast<gui::ogl_platform::texture*>(batch.texture);
				m_device.set_texture(t->get_platform_resource(), 0);

				m_device.draw
					(
					triangle_list,					//type
					s_quadOffset * VERTEX_PER_QUAD, // base_vertex_index Ok
					0,								// min_vertex_indexOk
					numQ*VERTEX_PER_QUAD,			//num_vertices OK
					0,								// Ok
					numQ*2							// Ok
					);

				s_quadOffset += (DWORD)numQ;

				if (batch.callbackInfo.window && batch.callbackInfo.afterRenderCallback)
				{
					m_shader->end_pass();
					m_shader->end();

					batch.callbackInfo.afterRenderCallback(batch.callbackInfo.window,
									batch.callbackInfo.dest, batch.callbackInfo.clip);

					// if it was not last batch
					if (b < m_num_batches -1)
						setRenderStates();
				}
			}
			
			m_shader->end_pass();
			m_shader->end();
		}

		/*************************************************************************
		setup states etc
		*************************************************************************/
		void renderer::initPerFrameStates(void)
		{
			// setup vertex stream
			m_device.set_stream_source(0, m_buffer, sizeof(QuadVertex));
			m_device.set_index_buffer(m_ibuffer);
		}

		void renderer::OnLostDevice()
		{
			if (m_shader)
				m_shader->on_device_lost();

			m_buffer.reset();

			m_texmanager.onDeviceLost();
		}

		void renderer::OnResetDevice(void)
		{
			if(!m_buffer)
			{
				// Recreate a vertex buffer
				m_buffer = vertex_buffer::create(
					m_device,
					m_vertexDeclaration,
					(VERTEXBUFFER_CAPACITY * sizeof(QuadVertex)), 
					resource::default,
					buffer::dynamic|buffer::write_only
					);

				if (!m_buffer)
				{
					throw std::exception("renderer::onDeviceReset - Failed to create the VertexBuffer for use by the renderer object.");
				}
			}
			if (m_shader)
				m_shader->on_device_reset();

			m_texmanager.onDeviceReset();
		}

		Size renderer::getViewportSize(void) const
		{
			// initialise renderer size
			view_port	vp;
			m_device.get_viewport(vp);
			return Size((float)vp.width, (float)vp.height);
		}

		TexturePtr	renderer::createTexture(const std::string& filename)
		{
			return m_texmanager.createTexture(filename);
		}

		TexturePtr	renderer::createTextureInstance(const std::string& filename) 
		{
			TexturePtr tex;
			if(filename.empty()) return TexturePtr();

			texture_ptr t;

			if (data_ptr data = m_filesystem->load_binary(filename))
			{
				t = rgde::render::texture::create(m_device, data->ptr, data->size);
			}

			if (!t) return TexturePtr();;

			tex.reset(new texture(*this, t));

			return tex;
		}

		TexturePtr renderer::createTexture(unsigned int buffWidth, unsigned int buffHeight, Texture::PixelFormat pixFormat)
		{
			TexturePtr tex;

			// calculate square size big enough for whole memory buffer
			unsigned int tex_size = buffWidth > buffHeight ? buffWidth : buffHeight;

			// create a texture
			// TODO: Check resulting pixel format and react appropriately.
			resource::format pixfmt;
			switch (pixFormat)
			{
			case Texture::PF_RGB:
				pixfmt = resource::r8g8b8;
				break;
			case Texture::PF_RGBA:
				pixfmt = resource::a8r8g8b8;
				break;
			default:
				throw std::exception("Failed to load texture from memory: Invalid pixelformat.");
				break;
			}

			rgde::render::texture_ptr platform_texture = rgde::render::texture::create(
				m_device, 
				tex_size, 
				tex_size, 
				1, 
				pixfmt
				);
			
			if (!platform_texture)
			{
				throw std::exception("Failed to load texture from memory: D3D Texture creation failed.");
			}
			else
			{		
				tex.reset(new texture(*this, platform_texture));
				m_texmanager.pushTexture(tex);
			}

			return tex;
		}

		TexturePtr	renderer::reloadTexture(TexturePtr p, const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, Texture::PixelFormat pixFormat)
		{
			texture_ptr platform_tex = static_cast<texture&>(*p).get_platform_resource();

			surface_ptr surface = platform_tex->get_surface(0);
			surface::lock_data ld;
			bool res = surface->lock(ld);

			if (!res)
			{
				platform_tex.reset();
				throw std::exception("Failed to load texture from memory: IDirect3DTexture9::LockRect failed.");
			}
			else
			{
				// copy data from buffer into texture
				unsigned long* dst = (unsigned long*)ld.bytes;
				unsigned long* src = (unsigned long*)buffPtr;

				// RGBA
				if (pixFormat == Texture::PF_RGBA)
				{
					for (unsigned int i = 0; i < buffHeight; ++i)
					{
						for (unsigned int j = 0; j < buffWidth; ++j)
						{
							//TODO: check for endian safety on non-MS platforms
							unsigned char r = (unsigned char)(src[j] & 0xFF);
							unsigned char g = (unsigned char)((src[j] >> 8) & 0xFF);
							unsigned char b = (unsigned char)((src[j] >> 16)  & 0xFF);
							unsigned char a = (unsigned char)((src[j] >> 24) & 0xFF);

							dst[j] = rgde::math::color(r, g, b, a).data;
						}

						dst += ld.pitch / sizeof(unsigned long);
						src += buffWidth;
					}
				}
				// RGB
				else
				{
					for (unsigned int i = 0; i < buffHeight; ++i)
					{
						for (unsigned int j = 0; j < buffWidth; ++j)
						{
							dst[j] = src[j];
						}

						dst += ld.pitch / sizeof(unsigned long);
						src += buffWidth;
					}
				}

				surface->unlock();
			}
			return p;
		}

		TexturePtr renderer::createTexture(const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, Texture::PixelFormat pixFormat)
		{
			//using namespace std;
			TexturePtr tex;

			// calculate square size big enough for whole memory buffer
			unsigned int tex_size = buffWidth > buffHeight ? buffWidth : buffHeight;

			// create a texture
			// TODO: Check resulting pixel format and react appropriately.
			resource::format pixfmt;
			switch (pixFormat)
			{
			case Texture::PF_RGB:
				pixfmt = resource::r8g8b8;
				break;
			case Texture::PF_RGBA:
				pixfmt = resource::a8r8g8b8;
				break;
			default:
				throw std::exception("Failed to load texture from memory: Invalid pixelformat.");
				break;
			}

			texture_ptr platform_tex = rgde::render::texture::create(
				m_device, 
				tex_size, 
				tex_size, 
				1, 
				pixfmt);

			if (!platform_tex)
			{
				throw std::exception("Failed to load texture from memory: D3D Texture creation failed.");
			}

			tex.reset(new texture(*this, platform_tex));

			reloadTexture(tex, buffPtr, buffWidth, buffHeight, pixFormat);
			
			m_texmanager.pushTexture(tex);

			return tex;
		}

		FontPtr	renderer::createFont(const std::string& name, const std::string& filename, unsigned int size)
		{
			return FontPtr(new FreeTypeFont(name, filename, size, *this));
		}

		void renderer::drawFromCache( base_window* window )
		{
			assert(window);
			QuadCacheMap::iterator i = m_mapQuadList.find(window);
			assert (i != m_mapQuadList.end());
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

				if (!m_num_quads  || quads[m_num_quads - 1].texture != quads[m_num_quads].texture ||
					m_needToAddCallback ||
					(m_num_batches && (m_num_quads - batches[m_num_batches - 1].startQuad + 1)*VERTEX_PER_QUAD >= VERTEXBUFFER_CAPACITY))
				{
					// terminate current batch if one:
					if (m_num_batches)
					{
						batches[m_num_batches - 1].numQuads = m_num_quads - batches[m_num_batches - 1].startQuad;
						if (!m_needToAddCallback)
						{
							m_callbackInfo.window = NULL;
							m_callbackInfo.afterRenderCallback = NULL;
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

		void renderer::beginBatching()
		{
			m_needToAddCallback = false;
			Renderer::beginBatching();
		}

		void renderer::endBatching()
		{
			if (!m_num_batches) return;
			Renderer::endBatching();
			if (!m_needToAddCallback)
			{
				m_callbackInfo.window = NULL;
				m_callbackInfo.afterRenderCallback = NULL;
			}
			m_needToAddCallback = false;
			m_batches[m_num_batches - 1].callbackInfo = m_callbackInfo;
		}
	}
}
