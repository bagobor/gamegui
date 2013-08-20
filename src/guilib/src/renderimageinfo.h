#pragma once

#include "rect.h"

namespace gui
{
	class Texture;

	struct RenderImageInfo
	{
		RenderImageInfo() : texture(0) {}
		Texture* texture;
		Rect pixel_rect;
		point offset;
		point crop;
	};
}