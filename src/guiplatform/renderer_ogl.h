#pragma once

#include <guilib/src/renderer.h>

namespace gui
{
	namespace ogl_platform
	{
		class TextureOGL : public Texture {
		public:
			enum CCTexture2DPixelFormat{
				//! 32-bit texture: RGBA8888
				kCCTexture2DPixelFormat_RGBA8888,
				//! 24-bit texture: RGBA888
				kCCTexture2DPixelFormat_RGB888,
				//! 16-bit texture without Alpha channel
				kCCTexture2DPixelFormat_RGB565,
				//! 8-bit textures used as masks
				kCCTexture2DPixelFormat_A8,
				//! 8-bit intensity texture
				kCCTexture2DPixelFormat_I8,
				//! 16-bit textures used as masks
				kCCTexture2DPixelFormat_AI88,
				//! 16-bit textures: RGBA4444
				kCCTexture2DPixelFormat_RGBA4444,
				//! 16-bit textures: RGB5A1
				kCCTexture2DPixelFormat_RGB5A1,
				//! 4-bit PVRTC-compressed texture: PVRTC4
				kCCTexture2DPixelFormat_PVRTC4,
				//! 2-bit PVRTC-compressed texture: PVRTC2
				kCCTexture2DPixelFormat_PVRTC2,


				//! Default texture format: RGBA8888
				kCCTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_RGBA8888,

				// backward compatibility stuff
				kTexture2DPixelFormat_RGBA8888 = kCCTexture2DPixelFormat_RGBA8888,
				kTexture2DPixelFormat_RGB888 = kCCTexture2DPixelFormat_RGB888,
				kTexture2DPixelFormat_RGB565 = kCCTexture2DPixelFormat_RGB565,
				kTexture2DPixelFormat_A8 = kCCTexture2DPixelFormat_A8,
				kTexture2DPixelFormat_RGBA4444 = kCCTexture2DPixelFormat_RGBA4444,
				kTexture2DPixelFormat_RGB5A1 = kCCTexture2DPixelFormat_RGB5A1,
				kTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_Default

			};

			enum
			{
				kFmtJpg = 0,
				kFmtPng,
				kFmtTiff,
				kFmtWebp,
				kFmtRawData,
				kFmtUnKnown
			}EImageFormat;

			struct TexParams {
				unsigned    minFilter;
				unsigned    magFilter;
				unsigned    wrapS;
				unsigned    wrapT;
			};

			void update(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format);
		};

		class RenderDeviceGL : public RenderDevice
		{
		public:
			RenderDeviceGL(filesystem_ptr fs, unsigned int max_quads);
			~RenderDeviceGL();

			TexturePtr createTexture(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format);
			TexturePtr createTexture(const std::string& filename);

			void renderImmediate(const QuadInfo& q);
			void render(const Batches& batches, const Quads& quads, Size scale);
			
			//void	initPerFrameStates();
			//void	constructor_impl(const Size& display_size);
			//void	setRenderStates();

		protected:	
			//rgde::render::device& m_device;
			//rgde::render::texture_ptr m_currTexture;
			//rgde::render::vertex_buffer_ptr	m_buffer;
			//rgde::render::index_buffer_ptr	m_ibuffer;
			//rgde::render::vertex_declaration_ptr m_vertexDeclaration;
			//int m_bufferPos;	

			//typedef rgde::render::effects::effect shader_effect;
			//typedef rgde::render::effects::param_ptr shader_handle;

			//typedef std::shared_ptr<shader_effect> shader_ptr;
			//shader_ptr				m_shader;

		protected:
			//shader_handle m_handleViewPortSize;
			//shader_handle m_handleGuiTexture;	 
			filesystem_ptr filesystem;
		};
	}
}

