#pragma once

namespace gmtl
{
	template<class DATA_TYPE, unsigned SIZE> class Vec;
	template<class DATA_TYPE, unsigned SIZE> class Point;
	template <typename DATA_TYPE> class Quat;
	template <typename DATA_TYPE, unsigned ROWS, unsigned COLS> class Matrix;	
}

namespace rgde
{
	namespace math
	{
		using namespace gmtl;

		typedef Vec<int, 2> vec2i;
		typedef Vec<float,2> vec2f;
		typedef Vec<double,2> vec2d;
		typedef Vec<int, 3> vec3i;
		typedef Vec<float,3> vec3f;
		typedef Vec<double,3> vec3d;
		typedef Vec<int, 4> vec4i;
		typedef Vec<float,4> vec4f;
		typedef Vec<double,4> vec4d;

		typedef Point<int, 2> point2i;
		typedef Point<float, 2> point2f;
		typedef Point<double, 2> point2d;
		typedef Point<int, 3> point3i;
		typedef Point<float, 3> point3f;
		typedef Point<double, 3> point3d;
		typedef Point<int, 4> point4i;
		typedef Point<float, 4> point4f;
		typedef Point<double, 4> point4d;

		typedef Quat<float> quatf;

		typedef Matrix<float, 2, 2> mat22f;
		typedef Matrix<float, 3, 3> mat33f;
		typedef Matrix<float, 3, 4> mat34f;
		typedef Matrix<float, 4, 4> mat44f;

		typedef Matrix<double, 2, 2> mat22d;		
		typedef Matrix<double, 3, 3> mat33d;		
		typedef Matrix<double, 3, 4> mat34d;		
		typedef Matrix<double, 4, 4> mat44d;

		struct rect;
		struct color;
	}
}