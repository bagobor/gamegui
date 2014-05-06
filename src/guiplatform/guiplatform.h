#pragma once

#include <memory>


namespace gui
{
	struct filesystem;
	typedef std::shared_ptr<filesystem> filesystem_ptr;

	class Renderer;
	typedef std::shared_ptr<Renderer> RendererPtr;

	namespace ogl_platform
	{
		RendererPtr CreateRenderer(filesystem_ptr fs, unsigned buff = 1024);
	}	
}
