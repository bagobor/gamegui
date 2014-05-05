#pragma once

#include <guilib/src/texture.h>
#include "renderer.h"

namespace gui
{
namespace ogl_platform
{
	class renderer;

	struct TextureGL : public Texture
	{	
		TextureGL(renderer& owner, size_t tex);
		virtual ~TextureGL();

		void free_platform_resource(void);
		virtual void calculateMetrics();

		size_t m_texture;
		std::string m_filename;
		std::string m_resourceGroup;
	};
}
}