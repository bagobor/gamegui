#pragma once

namespace gui
{
	/*!
	\brief
	Class used as a two dimensional vector (aka a point)
	*/
	struct vec2
	{
		vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

		inline vec2& operator*=(float f)
		{
			x *= f;
			y *= f;
			return *this;
		}

		inline vec2& operator/=(float f)
		{
			x /= f;
			y /= f;
			return *this;
		}

		inline vec2& operator*=(const vec2& vec)
		{
			x *= vec.x;
			y *= vec.y;
			return *this;
		}

		inline vec2& operator/=(const vec2& vec)
		{
			x /= vec.x;
			y /= vec.y;
			return *this;
		}

		inline vec2& operator+=(const vec2& vec)
		{
			x += vec.x;
			y += vec.y;
			return *this;
		}

		inline vec2& operator-=(const vec2& vec)
		{
			x -= vec.x;
			y -= vec.y;
			return *this;
		}

		inline vec2 operator+(const vec2& vec) const
		{
			return vec2(x + vec.x, y + vec.y);
		}

		inline vec2 operator-(const vec2& vec) const
		{
			return vec2(x - vec.x, y - vec.y);
		}

		inline vec2 operator*(const vec2& vec) const
		{
			return vec2(x * vec.x, y * vec.y);
		}

		inline bool operator==(const vec2& vec) const
		{
			return ((x == vec.x) && (y == vec.y));
		}

		inline bool operator!=(const vec2& vec) const
		{
			return !(operator==(vec));
		}

		float x, y;
	};


	inline void normalize(vec2& v)
	{
		v /= sqrtf(v.x*v.x + v.y*v.y);
	}

	inline vec2 make_normal(const vec2& v)
	{
		vec2 out = v;
		normalize(out);
		return out;
	}

	typedef	vec2	point;

	struct  vec3
	{
		vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
		float	x, y, z;
	};
}