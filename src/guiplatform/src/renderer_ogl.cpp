#include "stdafx.h"


#include <guilib/guilib.h>

#include <GL/GL.h>

#include "../renderer_ogl.h"
#include "opengl.h"

#include "png.h"

// fine tune :)
#define PixelAligned(x)	( ( (float)(int)(( x ) + (( x ) > 0.0f ? 0.5f : -0.5f)) ) - 0.5f )

const char* vertex_shader_src = R"glsl(
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

void main()
{
    gl_Position = CC_MVPMatrix * a_position;
	v_fragmentColor = a_color;
	v_texCoord = a_texCoord;
}

)glsl";

const char* pixel_shader_src = R"glsl(
#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform sampler2D CC_Texture0;

void main()
{
	gl_FragColor = vec4( v_fragmentColor.rgb,										// RGB from uniform
						v_fragmentColor.a * texture2D(CC_Texture0, v_texCoord).a	// A from texture & uniform
						);
}
)glsl";



namespace gui
{
	namespace ogl_platform
	{
		unsigned int bitsPerPixelForFormat(Texture::PixelFormat format)
		{
			switch (format) {
			case Texture::PF_RGB888:
			case Texture::PF_RGBA8888:
				return 32;
				
			case Texture::PF_RGB565:
			case Texture::PF_RGBA4444:
				return 16;

			case Texture::PF_PVRTC4:
				return 4;

			case Texture::PF_PVRTC2:
				return 2;
			}

			return (unsigned)-1;
		}

		TextureOGL::TextureOGL(RenderDeviceGL& r)
			: Texture(r),
			renderer(r),
			m_gl_handler(0)
		{
			glGenTextures(1, &m_gl_handler);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(0, m_gl_handler);
		}

		TextureOGL::~TextureOGL() {
			glDeleteTextures(1, &m_gl_handler);
		}

		bool TextureOGL::init(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format) {

			m_size.width = width;
			m_size.height = height;
			m_format = format;
			m_image_format = Texture::DF_RAW;

			unsigned int bitsPerPixel = 24;

			if (format != PF_RGB888) {
				bitsPerPixel = bitsPerPixelForFormat(format);
			}

			const unsigned int bytesPerRow = width * bitsPerPixel / 8;

			if (bytesPerRow % 8 == 0)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
			}
			else if (bytesPerRow % 4 == 0)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			}
			else if (bytesPerRow % 2 == 0)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
			}
			else
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			}

			switch (format) {
			//case Texture::PF_Default:
			case Texture::PF_RGBA8888:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				return true;

			case Texture::PF_RGB888:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				return true;

			case Texture::PF_RGBA4444:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
				return true;

			case Texture::PF_RGB565:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
				return true;

			}

			return false;
		}

		struct ImageSource
		{
			unsigned char* data;
			int size;
			int offset;
		};

		static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
		{
			ImageSource* isource = (ImageSource*)png_get_io_ptr(png_ptr);

			if ((int)(isource->offset + length) <= isource->size)
			{
				memcpy(data, isource->data + isource->offset, length);
				isource->offset += length;
			}
			else
			{
				png_error(png_ptr, "pngReaderCallback failed");
			}
		}

#define CC_BREAK_IF(cond)            if(cond) break
		bool TextureOGL::loadPNG(data_ptr data) {
			void * pData = data->ptr;
			int nDatalen = data->size;
#define PNGSIGSIZE  8
			bool bRet = false;
			png_byte        header[PNGSIGSIZE] = { 0 };
			png_structp     png_ptr = 0;
			png_infop       info_ptr = 0;

			png_byte* m_pData = nullptr;

			do
			{
				// png header len is 8 bytes
				CC_BREAK_IF(nDatalen < PNGSIGSIZE);

				// check the data is png or not
				memcpy(header, pData, PNGSIGSIZE);
				CC_BREAK_IF(png_sig_cmp(header, 0, PNGSIGSIZE));

				// init png_struct
				png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
				CC_BREAK_IF(!png_ptr);

				// init png_info
				info_ptr = png_create_info_struct(png_ptr);
				CC_BREAK_IF(!info_ptr);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_BADA && CC_TARGET_PLATFORM != CC_PLATFORM_NACL)
				CC_BREAK_IF(setjmp(png_jmpbuf(png_ptr)));
#endif

				// set the read call back function
				ImageSource imageSource;
				imageSource.data = (unsigned char*)pData;
				imageSource.size = nDatalen;
				imageSource.offset = 0;
				png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

				// read png header info

				// read png file info
				png_read_info(png_ptr, info_ptr);

				m_size.width = png_get_image_width(png_ptr, info_ptr);
				m_size.height = png_get_image_height(png_ptr, info_ptr);
				int m_nBitsPerComponent = png_get_bit_depth(png_ptr, info_ptr);
				png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);

				//CCLOG("color type %u", color_type);

				// force palette images to be expanded to 24-bit RGB
				// it may include alpha channel
				if (color_type == PNG_COLOR_TYPE_PALETTE)
				{
					png_set_palette_to_rgb(png_ptr);
				}
				// low-bit-depth grayscale images are to be expanded to 8 bits
				if (color_type == PNG_COLOR_TYPE_GRAY && m_nBitsPerComponent < 8)
				{
					png_set_expand_gray_1_2_4_to_8(png_ptr);
				}
				// expand any tRNS chunk data into a full alpha channel
				if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
				{
					png_set_tRNS_to_alpha(png_ptr);
				}
				// reduce images with 16-bit samples to 8 bits
				if (m_nBitsPerComponent == 16)
				{
					png_set_strip_16(png_ptr);
				}
				// expand grayscale images to RGB
				if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
				{
					png_set_gray_to_rgb(png_ptr);
				}

				// read png data
				// m_nBitsPerComponent will always be 8
				m_nBitsPerComponent = 8;

				//png_bytep* row_pointers = (png_bytep*)malloc( sizeof(png_bytep) * m_nHeight );
				png_bytep* row_pointers = (png_bytep*)alloca(sizeof(png_bytep)* m_size.height);

				png_read_update_info(png_ptr, info_ptr);
				png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);

				m_pData = new png_byte[rowbytes * m_size.height];
				CC_BREAK_IF(!m_pData);

				for (unsigned short i = 0; i < m_size.height; ++i)
				{
					row_pointers[i] = m_pData + i*rowbytes;
				}
				png_read_image(png_ptr, row_pointers);

				png_read_end(png_ptr, NULL);

				png_uint_32 channel = rowbytes / m_size.width;

				bool m_bHasAlpha = false; 

				if (channel == 4)
				{
					m_bHasAlpha = true;
					//unsigned int *tmp = (unsigned int *)m_pData;
					//for (unsigned short i = 0; i < m_nHeight; i++)
					//{
					//	for (unsigned int j = 0; j < rowbytes; j += 4)
					//	{
					//		*tmp++ = CC_RGB_PREMULTIPLY_ALPHA(row_pointers[i][j], row_pointers[i][j + 1],
					//			row_pointers[i][j + 2], row_pointers[i][j + 3]);
					//	}
					//}

					//m_bPreMulti = true;
				}


				bRet = true;
			} while (0);

			if (png_ptr)
			{
				png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
			}

			bRet = init(m_pData, m_size.width, m_size.height, Texture::PF_RGBA8888);

			delete[] m_pData;
			
			return bRet;
		}

		bool TextureOGL::init(const std::string& filename) {
			std::string lowerCase(filename);
			for (unsigned int i = 0; i < lowerCase.length(); ++i)
			{
				lowerCase[i] = tolower(lowerCase[i]);
			}

			if (std::string::npos != lowerCase.find(".png")) {
				m_image_format = Texture::DF_PNG;
			}

			if (m_image_format == Texture::DF_UNKNOWN)
				return false;

			data_ptr data = renderer.filesystem->load_binary(filename);
			if (!data) return false;

			m_filename = filename;

			switch (m_image_format)
			{
			//case gui::Texture::DF_FLAG_HARDWARE:
			//	break;
			//case gui::Texture::DF_JPG:
			//	break;
			//case gui::Texture::DF_PVR_APPLE:
			//	break;
			//case gui::Texture::DF_PVR_IMAGINATION:
			//	break;
			case gui::Texture::DF_PNG:
				return loadPNG(data);
				break;
			//case gui::Texture::DF_WEBP:
			//	break;
			//default:
			//	break;
			}
			return false;
		}

		void TextureOGL::update(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format) {
			init(data, width, height, format);
		}

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
		RenderDeviceGL::RenderDeviceGL(filesystem_ptr fs, unsigned int max_quads)
			: filesystem(fs)
			//: Renderer(fs)
		{
			viewport.x = viewport.y = viewport.w = viewport.h = 0;

			init_gl();
			//m_needToAddCallback = false;
			//Size size(getViewportSize());
			//constructor_impl(size);
		}


		TexturePtr RenderDeviceGL::createTexture(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format) {
			auto texture = std::make_shared<TextureOGL>(*this);
			if (!texture->init(data, width, height, format)) {
				return TexturePtr();
			}

			return texture;
		}

		TexturePtr RenderDeviceGL::createTexture(const std::string& filename) {
			auto texture = std::make_shared<TextureOGL>(*this);
			if (!texture->init(filename)) {
				return TexturePtr();
			}			

			return texture;
		}


		/*************************************************************************
		method to do work of constructor
		*************************************************************************/
		//void RendererGL::constructor_impl(const Size& display_size)
		//{
			//m_bufferPos     = 0;
			//m_originalsize = display_size;
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
			//m_maxTextureSize = 2048;//devCaps.MaxTextureWidth < devCaps.MaxTextureHeight ? devCaps.MaxTextureWidth : devCaps.MaxTextureHeight;

			//m_handleGuiTexture = m_shader->get_param("guitexture");
			//m_handleViewPortSize = m_shader->get_param("viewportsize");
		//}


		/*************************************************************************
		Destructor
		*************************************************************************/
		RenderDeviceGL::~RenderDeviceGL(void)
		{
		}

		/*************************************************************************
		render a quad directly to the display		
		*************************************************************************/

		namespace 
		{
			// return value = buff offset in QuadInfo
			/*__inline */__forceinline unsigned int fill_vertex(const QuadInfo& q, QuadVertex*& v, float scaleX, float scaleY)
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
		void RenderDeviceGL::renderImmediate(const QuadInfo& q)
		{
			int i = 5;
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
		
		//void RendererGL::setRenderStates()
		//{
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
		//}
		/*************************************************************************
		perform final rendering for all queued renderable quads.
		*************************************************************************/
		void RenderDeviceGL::render(const Batches& batches, const Quads& quads, Size scale)
		{
			int i = 5;
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
		//void RendererGL::initPerFrameStates(void)
		//{
		//	// setup vertex stream
		//	m_device.set_stream_source(0, m_buffer, sizeof(QuadVertex));
		//	m_device.set_index_buffer(m_ibuffer);
		//}

		//TexturePtr	RendererGL::updateTexture(TexturePtr p, const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, Texture::PixelFormat pixFormat)
		//{
			//return TexturePtr();
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
		//}

	}
}
