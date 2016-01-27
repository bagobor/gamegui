#pragma once

#include <guilib/guilib.h>
#include <guilib/src/renderer.h>


typedef std::shared_ptr<struct gpu_program> gpu_program_ptr;
typedef std::shared_ptr<struct mesh> mesh_ptr;

namespace gui
{
	namespace ogl_platform
	{
		struct RenderDeviceGL;

		class TextureOGL : public Texture {
		public:
			struct TexParams {
				unsigned    minFilter;
				unsigned    magFilter;
				unsigned    wrapS;
				unsigned    wrapT;
			};

			explicit TextureOGL(RenderDeviceGL& r);
			~TextureOGL();
			
			void update(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format);

			bool init(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format);
			bool init(const std::string& filename);

			void bind(size_t slot);

			bool loadPNG(data_ptr data);

			unsigned m_gl_handler;
			RenderDeviceGL& renderer;
		};

		class RenderDeviceGL : public RenderDevice
		{
		public:
			RenderDeviceGL(filesystem_ptr fs, unsigned int max_quads);
			~RenderDeviceGL();

			TexturePtr createTexture(const void* data, unsigned int width, unsigned int height, Texture::PixelFormat format);
			TexturePtr createTexture(const std::string& filename);

			void renderImmediate(const QuadInfo& q, Texture* texture, bool isAdditive);
			void render(const Batches& batches, const Quads& quads, size_t num_batches, Size scale);
			
			filesystem_ptr filesystem;
			gpu_program_ptr m_shader;
			mesh_ptr m_mesh;

			//for Immediate Drawing of sprites
			mesh_ptr m_small_mesh;
		};
	}
}

