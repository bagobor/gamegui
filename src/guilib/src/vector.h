#pragma once

namespace gui
{
	/*!
	\brief
	Class used as a two dimensional vector (aka a point)
	*/
	class  vec2
	{
	public:
		vec2() {}
		vec2(float x, float y) : x(x), y(y) {}

		vec2& operator*=(float f)
		{
			x *= f;
			y *= f;
			return *this;
		}

		vec2& operator/=(float f)
		{
			x /= f;
			y /= f;
			return *this;
		}

		vec2& operator*=(const vec2& vec)
		{
			x *= vec.x;
			y *= vec.y;
			return *this;
		}

		vec2& operator/=(const vec2& vec)
		{
			x /= vec.x;
			y /= vec.y;
			return *this;
		}

		vec2& operator+=(const vec2& vec)
		{
			x += vec.x;
			y += vec.y;
			return *this;
		}

		vec2& operator-=(const vec2& vec)
		{
			x -= vec.x;
			y -= vec.y;
			return *this;
		}

		vec2	operator+(const vec2& vec) const
		{
			return vec2(x + vec.x, y + vec.y);
		}

		vec2	operator-(const vec2& vec) const
		{
			return vec2(x - vec.x, y - vec.y);
		}

		vec2	operator*(const vec2& vec) const
		{
			return vec2(x * vec.x, y * vec.y);
		}

		bool	operator==(const vec2& vec) const
		{
			return ((x == vec.x) && (y == vec.y));
		}

		bool	operator!=(const vec2& vec) const
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

	/*!
	\brief
	point class
	*/
	typedef	vec2	point;


	/*!
	\brief
	Class used as a three dimensional vector
	*/
	class  Vector3
	{
	public:
		Vector3(void) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		float	x, y, z;
	};
}