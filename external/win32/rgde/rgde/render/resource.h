#pragma once


namespace rgde
{
	namespace render
	{
		class resource
		{
		public:

#define MKFRCC(ch0, ch1, ch2, ch3) ((unsigned)(unsigned char)(ch0) | \
			((unsigned)(unsigned char)(ch1) << 8) | \
			((unsigned)(unsigned char)(ch2) << 16) | \
			((unsigned)(unsigned char)(ch3) << 24 )) 

			enum format
			{
				unknown = 0,
				r8g8b8 = 20,
				a8r8g8b8 = 21,
				x8r8g8b8 = 22,

				dxt1 = MKFRCC('D','X','T','1'),
				dxt2 = MKFRCC('D','X','T','2'),
				dxt3 = MKFRCC('D','X','T','3'),
				dxt4 = MKFRCC('D','X','T','4'),
				dxt5 = MKFRCC('D','X','T','5'),

				d16_locable = 70,
				d32 = 71,
				d24s8 = 75,
				d24x8 = 77,
				d16 = 80,

				vertex_data,
				index16,
				index32,

				// -- Floating point surface formats --
				// s10e5 formats (16-bits per channel)
				r16f = 111,
				g16r16f = 113,
				a16b16g16r16f = 113,

				// IEEE s23e8 formats (32-bits per channel)
				r32f = 114,
				g32r32f = 115,
				a32b32g32r32f = 116
			};

#undef MKFRCC

			enum type
			{
				surface = 1,
				volume,
				texture,
				volume_texture,
				cube_texture,
				vertexbuffer,
				indexbuffer
			};

			enum pool
			{
				default,
				managed,
				systemmem
			};

			inline type		get_type() const {return m_type;}
			inline format	get_format() const {return m_format;}
			inline pool		get_pool() const {return m_pool;}

		protected:
			resource(type res_type, format res_format, pool res_pool);

		protected:
			type	m_type;
			pool	m_pool;
			format	m_format;
		};
	}
}