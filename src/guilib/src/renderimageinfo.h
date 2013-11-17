#pragma once

#include "rect.h"
#include "imagesetmanager.h"

namespace gui
{
	class Texture;

	struct RenderImageInfo
	{
		RenderImageInfo() : texture(0), isAdditiveBlend(false) {}
		Texture* texture;
		Rect pixel_rect;
		point offset;
		point crop;
		bool  isAdditiveBlend;
	};
}