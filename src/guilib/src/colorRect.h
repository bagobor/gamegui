#pragma once

#include "color.h"

namespace gui
{
	struct  ColorRect
	{
		ColorRect();
		ColorRect(const Color& col);
		ColorRect(const Color& top_left, const Color& top_right, const Color& bottom_left, 
			const Color& bottom_right);

		void setAlpha(float alpha);
		void setTopAlpha(float alpha);
		void setBottomAlpha(float alpha);
		void setLeftAlpha(float alpha);
		void setRightAlpha(float alpha);

		bool isMonochromatic() const;

		ColorRect getSubRectangle( float left, float right, float top, float bottom ) const;

		Color getColorAtPoint( float x, float y ) const;
		void setColors(const Color& col);

		void modulateAlpha(float alpha);

		ColorRect& operator*=(const ColorRect& other);

		//ColorRect component colors
		Color m_top_left;
		Color m_top_right;
		Color m_bottom_left;
		Color m_bottom_right;
	};
}