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
			
			//void	initPerFrameStates();
			//void	constructor_impl(const Size& display_size);
			//void	setRenderStates();


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


			//shader_handle m_handleViewPortSize;
			//shader_handle m_handleGuiTexture;	 
			filesystem_ptr filesystem;
			gpu_program_ptr m_shader;
			mesh_ptr m_mesh;
		};
	}
}

