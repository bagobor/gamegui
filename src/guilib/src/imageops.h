#pragma once

#include <string>

namespace gui
{
	/// @brief - type to render
	enum ImageOps
	{
		Stretch,
		Tile
	};

	ImageOps StringToImageOps(const std::string& str);
	std::string ImageOpsToString(ImageOps op);
}