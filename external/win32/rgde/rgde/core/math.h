#pragma once

#include <rgde/core/math/forward.h>
#include <gmtl/gmtl.h>

#include <rgde/core/math/rect.h>
#include <rgde/core/math/color.h>

namespace rgde
{
	namespace math
	{

		/// make perspective LH matrix
		inline mat44f make_perspective(float fovy, float aspect, float zn, float zf)
		{
			float hfov = fovy/2;
			float yscale = ::cos(hfov)/::sin(hfov);
			float xscale = yscale / aspect;

			mat44f out;

			out.set(xscale,	0,			0,			0,
					0,		yscale,		0,			0, 
					0,		0,		zf/(zf-zn),		1,
					0,		0,		-zn*zf/(zf-zn), 0);

			gmtl::transpose(out);

			return out;
		}

		inline mat44f make_lookat(const vec3f& eye, const vec3f& at, const vec3f& up)
		{
			//////////////////////////////////////////////////////////////////////////
			// LH
			vec3f zaxis = (at - eye); 
			normalize(zaxis);

			vec3f xaxis = makeCross(up, zaxis);
			normalize(xaxis);

			vec3f yaxis = makeCross(zaxis, xaxis);

			mat44f out;

			out.set(
				xaxis[0],          yaxis[0],          zaxis[0],         0,
				xaxis[1],          yaxis[1],          zaxis[1],         0,
				xaxis[2],          yaxis[2],          zaxis[2],         0,
				-dot(xaxis, eye),  -dot(yaxis, eye),  -dot(zaxis, eye),		1);

			transpose(out);

			return out;
		}
	}
}