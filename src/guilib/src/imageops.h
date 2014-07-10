#pragma once

#include <string>

namespace gui
{
	/// @brief - type to render
	enum class ImageOps
	{
		Stretch = 0,
		Tile,
		None,
		Zoom,
		Center
	};

	ImageOps StringToImageOps(const std::string& str);
	const std::string& ImageOpsToString(ImageOps op);
}