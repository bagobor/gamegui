#pragma once

#include <rgde/render/render_forward.h>
#include <rgde/core/file_system.h>

namespace gui
{
	struct filesystem;
	typedef boost::shared_ptr<filesystem> filesystem_ptr;

	class Renderer;

	namespace rgde_platform
	{
		Renderer* CreateRenderer(rgde::render::device& dev, filesystem_ptr fs, unsigned buff = 1024);
	}	
}
