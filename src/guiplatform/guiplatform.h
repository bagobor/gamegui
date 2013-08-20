#pragma once

#include <memory>


namespace gui
{
	struct filesystem;
	typedef std::shared_ptr<filesystem> filesystem_ptr;

	class Renderer;

	namespace rgde_platform
	{
		Renderer* CreateRenderer(rgde::render::device& dev, filesystem_ptr fs, unsigned buff = 1024);
	}	
}
