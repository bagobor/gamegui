#include "stdafx.h"

#include <guilib/guilib.h>
#include <GL/GL.h>
#include <GL/glu.h>

#include "../renderer_ogl.h"
#include "opengl.h"

#include "png.h"
#include "render_gl.h"


// fine tune :)
// for directx
//#define PixelAligned(x)	( ( (float)(int)(( x ) + (( x ) > 0.0f ? 0.5f : -0.5f)) ) - 0.5f )
// for opengl, reduce controls blurring
#define PixelAligned(x)	((float)((int)( x )))


void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		const char* err_string = (const char*)gluErrorString(err);
		printf("[GL ERROR]  %s [%08x]\n%s\nat %s:%i\n", err_string, err, stmt, fname, line);
		abort();
	}
}

#ifdef _DEBUG
#define GL_CHECK(stmt) do { \
	stmt; \
	CheckOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)
#else
#define GL_CHECK(stmt) stmt
#endif

//#define GLErrCheck() 
//GLenum errCode = glGetError();
//if (errCode != GL_NO_ERROR) {
//	const char* error_str = (const char*)gluErrorString(errCode);
//	printf("[GL ERROR] %s", error_str);
//}


const char* vertex_shader_src =
"#version 330 \n"
" \n"
"in vec2 a_position;\n"
"in vec2 a_texCoord;\n"
"in vec4 a_color;\n"

"#ifdef GL_ES\n"
"uniform lowp vec2 v_viewportSize;\n"
"out lowp vec4 v_fragmentColor;\n"
"out mediump vec2 v_texCoord;\n"
"#else\n"
"uniform vec2 v_viewportSize; \n"
"out vec4 v_fragmentColor; \n"
"out vec2 v_texCoord; \n"
"#endif\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(\n"
//"		a_position.x,\n"
//"		a_position.y,\n"
// "		a_position.x / v_viewportSize.x,\n"
//"		a_position.y / v_viewportSize.y,\n"
"		(2.0f * a_position.x / v_viewportSize.x) - 1.0f,\n"
"		1.0f - (2.0f * a_position.y / v_viewportSize.y),\n"
"		0.5f,\n"
"		1.0f\n"
"	);\n"
"\n"
"	v_fragmentColor = a_color;\n"
"	v_texCoord = a_texCoord;\n"
"}\n";


const char* fragment_shader_src =
"#version 330\n"
"\n"
"#ifdef GL_ES\n"
"precision lowp float;\n"
"#endif\n"
"\n"
"in vec4 v_fragmentColor;\n"
"in vec2 v_texCoord;\n"
"uniform sampler2D Texture0;\n"
"out vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"	color = v_fragmentColor * texture2D(Texture0, v_texCoord);\n"
"}\n";

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
			GL_CHECK(glGenTextures(1, &m_gl_handler));
			GL_CHECK(glActiveTexture(GL_TEXTURE0));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_gl_handler));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		}

		TextureOGL::~TextureOGL() {
			GL_CHECK(glDeleteTextures(1, &m_gl_handler));
		}

		void TextureOGL::bind(size_t slot){
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_gl_handler));
		}

		bool TextureOGL::init(const void* data, size_t size, unsigned int width, unsigned int height, Texture::PixelFormat format) {

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
				GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 8));
			}
			else if (bytesPerRow % 4 == 0)
			{
				GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
			}
			else if (bytesPerRow % 2 == 0)
			{
				GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 2));
			}
			else
			{
				GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
			}

			bind(0);

			switch (format) {
			//case Texture::PF_Default:
			case Texture::PF_RGBA8888:
				GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
				return true;

			case Texture::PF_RGB888:
				GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
				return true;

			case Texture::PF_RGBA4444:
				GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data));
				return true;

			case Texture::PF_RGB565:
				GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data));
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
			size_t data_size = 0;
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

				// Expanded earlier for grayscale, now take care of palette and rgb
				if (m_nBitsPerComponent < 8) {
					png_set_packing(png_ptr);
				}
				// update info
				png_read_update_info(png_ptr, info_ptr);
				m_nBitsPerComponent = png_get_bit_depth(png_ptr, info_ptr);
				color_type = png_get_color_type(png_ptr, info_ptr);


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
				png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);

				data_size = rowbytes * m_size.height * sizeof(png_byte);
				m_pData = new png_byte[rowbytes * m_size.height];
				CC_BREAK_IF(!m_pData);

				for (unsigned short i = 0; i < m_size.height; ++i)
				{
					row_pointers[i] = m_pData + i*rowbytes;
				}
				png_read_image(png_ptr, row_pointers);
				png_read_end(png_ptr, NULL);

				if (row_pointers != nullptr)
				{
					//free(row_pointers);
				}

				png_uint_32 channel = rowbytes / m_size.width;

				bool m_bHasAlpha = (channel == 4);

				bRet = true;
			} while (0);

			if (png_ptr)
			{
				png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
			}

			bRet = init(m_pData, data_size, m_size.width, m_size.height, Texture::PF_RGBA8888);

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

		void TextureOGL::update(const void* data, size_t size, unsigned int width, unsigned int height, Texture::PixelFormat format) {
			init(data, size, width, height, format);
		}

		struct QuadVertex
		{
			float x, y, tu1, tv1;		//!< The transformed position for the vertex.
			unsigned int diffuse;		//!< colour of the vertex
		};

		enum
		{
			VERTEX_PER_QUAD = 4,
			VERTEX_PER_TRIANGLE = 3,
			QUADS_BUFFER = 8000,
			VERTEXBUFFER_CAPACITY = QUADS_BUFFER * VERTEX_PER_QUAD,
			INDEXBUFFER_CAPACITY = QUADS_BUFFER * 6,
		};

		vertex_atrib va[] = {
			{ "a_position", 2, GL_FLOAT, false, sizeof(QuadVertex), 0 },
			{ "a_texCoord", 2, GL_FLOAT, false, sizeof(QuadVertex), sizeof(float)* 2 },
			{ "a_color", 4, GL_UNSIGNED_BYTE, true, sizeof(QuadVertex), sizeof(float)* 4 },
			{ 0 }
		};

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

		RenderDeviceGL::RenderDeviceGL(filesystem_ptr fs, unsigned int max_quads)
			: filesystem(fs)
		{
			viewport.x = viewport.y = 0;
			viewport.w = 1024;
			viewport.h = 768;

			init_gl();			

			m_shader = gpu_program::create(vertex_shader_src, fragment_shader_src);
			
			unsigned short* data = new unsigned short[INDEXBUFFER_CAPACITY];

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

			auto ibuffer = index_buffer::create(INDEXBUFFER_CAPACITY*sizeof(unsigned short), data);
			auto vbuffer = vertex_buffer::create(VERTEXBUFFER_CAPACITY * sizeof(QuadVertex), nullptr, true);

			delete[] data;

			m_mesh = mesh::create(va, vbuffer, ibuffer);
			m_mesh->setShader(m_shader);

			auto small_vbuffer = vertex_buffer::create(4 * sizeof(QuadVertex), nullptr, true);
			m_small_mesh = mesh::create(va, small_vbuffer, ibuffer);
			m_small_mesh->setShader(m_shader);
		}

		TexturePtr RenderDeviceGL::createTexture(const void* data, size_t size, unsigned int width, unsigned int height, Texture::PixelFormat format) {
			auto texture = std::make_shared<TextureOGL>(*this);
			if (!texture->init(data, size, width, height, format)) {
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

		RenderDeviceGL::~RenderDeviceGL(void)
		{
		}

		void RenderDeviceGL::renderImmediate(const QuadInfo& q, Texture* texture, bool isAdditive)
		{
			if (!m_small_mesh)
				return;

			GL_CHECK(glDisable(GL_CULL_FACE));
			GL_CHECK(glDepthMask(GL_FALSE));
			GL_CHECK(glEnable(GL_BLEND));
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			GL_CHECK(glBlendFunc(GL_SRC_ALPHA, (isAdditive ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA)));
			GL_CHECK(glEnable(GL_DEPTH_TEST));
			//GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			//GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

			//GLuint vao;
			//glGenVertexArrays(1, &vao);			
		
			m_shader->begin();
			{

				auto vp = getViewport();
				glm::vec2 viewport_size = { vp.w,vp.h };
				m_shader->set("v_viewportSize", viewport_size);
				m_shader->set("Texture0", (TextureOGL*)texture);

				static QuadVertex buffmem[VERTEX_PER_QUAD];

				float scaleX = 1.f;
				float scaleY = 1.f;
				float m_autoScale = true;
				//if (m_autoScale)
				//{
				//	const Size viewport(viewport_size.x, viewport_size.y);
				//	scaleX = viewport.width / m_originalsize.width;
				//	scaleY = viewport.height / m_originalsize.height;
				//}

				QuadVertex* temp_ptr = (QuadVertex*)buffmem;
				unsigned int vert_filled = fill_vertex(q, temp_ptr, scaleX, scaleY);

				size_t m_bufferPos = vert_filled;

				// if bufferPos is 0 there is no data in the buffer and nothing to render
				if (m_bufferPos > 0) {
					m_small_mesh->update_vb(sizeof(QuadVertex)* 4, buffmem, true);

					typedef unsigned short uint16;
					const unsigned int prim_count = 2 * m_bufferPos / VERTEX_PER_QUAD;

					m_small_mesh->bind();

					GL_CHECK(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0));

					// reset buffer position to 0...
					m_bufferPos = 0;
					m_small_mesh->unbind();
				}

			}
			m_shader->end();
		}
		
		QuadVertex buffmem[10000 * VERTEX_PER_QUAD];

		void RenderDeviceGL::render(const Batches& _batches, const Quads& _quads, size_t num_batches, Size scale)
		{
			if (!m_mesh || !m_shader) return;

			//setRenderStates();
			Texture* cur_texture = nullptr;

			float scaleX = scale.width;
			float scaleY = scale.height;

			auto vp = getViewport();
			glm::vec2 viewport_size = { vp.w,vp.h };

			//if(m_autoScale)
			//{
			//	Size& viewport = getViewportSize();
			//	scaleX = viewport.width / m_originalsize.width;
			//	scaleY = viewport.height / m_originalsize.height;
			//}
			GL_CHECK(glEnable(GL_BLEND));
			

			static unsigned long s_quadOffset = 0;	// buffer offset in quads			
			
			static const unsigned int quad_size = VERTEX_PER_QUAD * sizeof(QuadVertex);

			const BatchInfo* batches = &_batches.front();
			const QuadInfo* quads = &_quads.front();
					
			m_shader->begin();
			for (std::size_t b = 0; b < num_batches; ++b)
			{
				const BatchInfo& batch = batches[b];
				if ( VERTEX_PER_QUAD * (batch.numQuads + s_quadOffset) >= VERTEXBUFFER_CAPACITY)
					s_quadOffset = 0;

				GL_CHECK(glBlendFunc(GL_SRC_ALPHA, (batch.isAdditiveBlend ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA)));
				//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

				//buffmem = (QuadVertex*)m_buffer->lock
				//	(
				//	s_quadOffset * quad_size,
				//	batches[b].numQuads * quad_size,
				//	buffer::nooverwrite
				//	);

				//if (!buffmem)
				//	break;

				const std::size_t numQ = batch.numQuads;

				QuadVertex* buff = (QuadVertex*)&(buffmem[0]);
				const QuadInfo* quad = &quads[batch.startQuad];
				for (std::size_t q = 0; q < numQ; ++q, ++quad)
				{
					fill_vertex(*quad, buff, scaleX, scaleY);
				}


				TextureOGL* texture = (TextureOGL*)batch.texture;
				m_shader->set("Texture0", texture);
				
				m_mesh->update_vb(sizeof(QuadVertex)* 4 * numQ, buffmem, true);				

				m_shader->set("v_viewportSize", viewport_size);

				m_mesh->bind();
				
				GL_CHECK(glDrawElements(GL_TRIANGLES, numQ * 6, GL_UNSIGNED_SHORT, 0));

				m_mesh->unbind();

				s_quadOffset += (DWORD)numQ;

				if (batch.callbackInfo.window && batch.callbackInfo.afterRenderCallback)
				{
					batch.callbackInfo.afterRenderCallback(batch.callbackInfo.window, batch.callbackInfo.dest, batch.callbackInfo.clip);
				}
			}
			
			m_shader->end();
		}
	}
}
