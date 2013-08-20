#pragma once

#include <rgde/core/math/forward.h>

namespace rgde
{
	namespace math
	{
		struct rect
		{
			rect();
			rect(float x, float y, float w, float h);
			rect(const vec2f& pos, const vec2f& s);

			inline const vec2f& get_size() const;
			inline const vec2f& get_position() const;

			inline const vec2f& get_top_left() const;
			inline vec2f get_top_right() const;
			inline vec2f get_bottom_left() const;
			inline vec2f get_bottom_right() const;

			inline bool is_inside(const vec2f& point) const;

			union
			{
				struct {
					vec2f position, size;
				};
				struct {
					float x,y,w,h;
				};
			};
		};

		const vec2f& rect::get_size() const	
		{
			return size;
		}

		const vec2f& rect::get_position() const	
		{ 
			return position;
		}

		const vec2f& rect::get_top_left()	const	
		{ 
			return position; 
		}

		vec2f rect::get_top_right() const	
		{
			return vec2f(x+w, y); 
		}

		vec2f rect::get_bottom_left() const	
		{
			return vec2f(x, y+h); 
		}

		vec2f rect::get_bottom_right() const
		{ 
			return vec2f(x+w, y+h); 
		}

		bool rect::is_inside(const vec2f& point) const
		{
			if (position[0] > point[0] || position[1] > point[1]) 
				return false;
			else if (point[0] > position[0] + size[0]) 
				return false;
			else if (point[1] > position[1] + size[1]) 
				return false;
			else
				return true;
		}
	}
}