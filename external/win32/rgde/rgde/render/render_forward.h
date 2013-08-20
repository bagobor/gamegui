#pragma once

#include <boost/shared_ptr.hpp>

namespace rgde
{
	namespace render
	{
		enum img_type
		{
			img_bmp = 0,
			img_jpg = 1,
			img_tga = 2,
			img_png = 3,
			img_dds = 4,
			img_hdr = 7
		};

		enum cube_face
		{
			face_positivex = 0,
			face_negativex = 1,
			face_positivey = 2,
			face_negativey = 3,
			face_positivez = 4,
			face_negativez = 5
		};


		typedef boost::shared_ptr<class device> device_ptr;
		typedef boost::shared_ptr<class font> font_ptr;
		typedef boost::shared_ptr<class resource> resource_ptr;
		typedef boost::shared_ptr<class surface> surface_ptr;
		typedef boost::shared_ptr<class texture> texture_ptr;
		typedef boost::shared_ptr<class vertex_declaration> vertex_declaration_ptr;
		typedef boost::shared_ptr<class vertex_buffer> vertex_buffer_ptr;
		typedef boost::shared_ptr<class index_buffer> index_buffer_ptr;
	}
}