#pragma once

#include <rgde/core/math/forward.h>
#include <rgde/core/types.h>

namespace rgde
{
	namespace math
	{
		struct color
		{
			color(const vec3f& v);
			color(const vec4f& v);

			color(unsigned long c = 0);
			color(const color& c);
			color(const color& c, float alpha);
			color(uchar r, uchar g, uchar b, uchar a);

			void set(uchar r, uchar g, uchar b, uchar a);
			void set(float r, float g, float b, float a);

			color& operator=(const vec4f& vec);

			operator vec4f() const;
			inline operator unsigned int() const {return data;}

			union
			{
				unsigned int data;
				struct {uchar b, g, r, a;};
			};

			static const color DarkGreen;	//(0,127,0,255);
			static const color Green;		//(0,255,0,255);
			static const color DarkRed;		//(127,0,0,255);
			static const color Red;			//(255,0,0,255);
			static const color DarkBlue;	//(0,0,127,255);
			static const color Blue;		//(0,0,255,255);		
			static const color White;		//(255,255,255,255);
			static const color Grey;		//(127,127,127,255);
			static const color Black;		//(0,0,0,255);
		};

		color operator*( const color& color, float factor );
		color operator+( const color& c1, const color& c2 );
		color operator-( const color& c1, const color& c2 );
	}
}