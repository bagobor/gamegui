#pragma once

#include <rgde/render/render_forward.h>
#include <rgde/render/resource.h>
#include <string>

namespace rgde
{
	namespace core
	{
		namespace vfs
		{
			typedef boost::shared_ptr<struct istream> istream_ptr;
			typedef boost::shared_ptr<struct ostream> ostream_ptr;
		}
	}

	namespace render
	{

		enum multisample_type
		{
			ms_none	   =  0,
			ms_2       =  2,
			ms_3       =  3,
			ms_4       =  4,
			ms_5       =  5,
			ms_6       =  6,
			ms_7       =  7,
			ms_8       =  8,
		};

		class surface : public resource
		{
		public:
			class surface_impl;
			typedef boost::shared_ptr<surface_impl> impl_ptr;
			
			static surface_ptr create_rt(device& dev, size_t width, size_t heigh, 
				resource::format format, multisample_type sm_type, bool locable);

			/// for internal use
			static surface_ptr create(impl_ptr impl) ;

			size_t get_width() const;
			size_t get_height() const;

			struct lock_data 
			{
				/// Pitch of surface, in bytes. 
				int pitch; 
				/// Pointer to the locked bits.
				void *bytes;
			};

			bool lock(lock_data& data) const;
			bool unlock() const;

			impl_ptr		get_impl()		 {return m_impl;}
			const impl_ptr& get_impl() const {return m_impl;}

		protected:
			surface(impl_ptr impl, resource::format res_format, resource::pool res_pool);

			impl_ptr m_impl;
		};

		// Usage Flags
		enum texture_usage
		{
			usage_default = 0, 
			usage_rendertarget = (0x00000001L),
			usage_depthstencil = (0x00000002L),
			usage_dynamic = (0x00000200L),
		};

		class texture : public resource
		{
		public:
			class texture_impl;
			typedef boost::shared_ptr<texture_impl> impl_ptr;

			static texture_ptr create(device& dev, void* data, size_t size);
			static texture_ptr create(device& dev, core::vfs::istream_ptr file);

			static texture_ptr create(device& dev, size_t width, size_t heigh, size_t num_levels,
									resource::format format, texture_usage usage = usage_default,
									resource::pool pool = resource::managed);

			impl_ptr		get_impl()		 {return m_impl;}
			const impl_ptr& get_impl() const {return m_impl;}

			/// Returns the number of texture mip levels
			size_t get_level_count() const;

			/// returns empty ptr for not 2d texture
			surface_ptr get_surface(size_t surface_level);
			/// returns empty ptr for not cube texture
			surface_ptr get_surface(cube_face face, size_t surface_level);

			size_t get_width() const;
			size_t get_height() const;
			/// valid only for 3d textures
			size_t get_depth() const;

		protected:			
			texture(impl_ptr impl, resource::type type, resource::format format);

		protected:
			impl_ptr m_impl;
		};
	}
}