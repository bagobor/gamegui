#pragma once

#include <rgde/core/types.h>
#include <rgde/core/windows_forward.h>
#include <rgde/render/render_forward.h>
#include <rgde/core/file_system_forward.h>

#include <boost/noncopyable.hpp>
#include <list>

namespace rgde
{
	namespace render
	{
		enum cull_mode
		{
			cull_none = 1, 
			cull_cw,
			cull_ccw			
		};

		enum transform_type 
		{
			view_transform,
			projection_transform,
			world_transform,
			texture0_transform,
			texture1_transform,
			texture2_transform,
			texture3_transform
		};

		enum primitive_type 
		{
			point_list = 1,
			line_list,
			line_strip,
			triangle_list,
			triangle_strip,
			triangle_fan
		};

		enum blend_mode
		{
			blend_one,
			blend_zero,
			blend_src_color,
			blend_invsrc_color,
			blend_src_alpha,
			blend_invsrc_alpha,
			blend_dest_alpha,
			blend_invdest_alpha,
			blend_dest_color,
			blend_invdest_color
		};

		enum texture_addr
		{
			mode_wrap            = 1,
			mode_mirror          = 2,
			mode_clamp           = 3,
			mode_border          = 4,
		};
	
		struct view_port
		{
			size_t      x;
			size_t      y;            /* Viewport Top left */
			size_t      width;
			size_t      height;       /* Viewport Dimensions */
			float       minz;         /* Min/max of clip Volume */
			float       maxz;
		};

		class device : boost::noncopyable
		{
			friend class device_object;
		public:
			class device_impl;
			typedef boost::scoped_ptr<device_impl> pimpl;

			device(core::windows::handle hwnd, core::vfs::system& vfs, bool windowed = true);
			virtual ~device();

			void set_lighting(bool enable);
			void set_ztest(bool enable);
			void set_cull_mode(cull_mode mode);
			void set_alpha_test(bool enable);
			void set_alpha_blend(bool enable);
			void set_transform(transform_type type, const math::mat44f& m);
			void set_texture(texture_ptr texture, size_t index);
			void set_blend_mode(blend_mode src, blend_mode dest);

			void draw(primitive_type type, uint num_vertices, uint primitive_count, const void* vertex_data, uint vertex_stride,
								const uint16* index_data);
			void draw(primitive_type type, uint primitive_count, void* data, uint stride);
			void draw(primitive_type type, uint start_vertex, uint primitive_count);
			void draw(primitive_type type, int base_vertex_index, uint min_vertex_index,
					  uint num_vertices, uint start_index, uint primitive_count);

			void set_index_buffer(index_buffer_ptr ib);
			void set_stream_source(uint stream_number, vertex_buffer_ptr stream_data, uint stride, uint bytes_offset = 0);
			void set_decl(vertex_declaration_ptr& decl);

			void set_tex_adressu(size_t stage, texture_addr mode);
			void set_tex_adressv(size_t stage, texture_addr mode);

			void clear(unsigned int color, float depth = 1.0f);
			bool frame_begin();
			bool frame_end();
			bool present();

			void get_viewport(view_port& vp);
			void set_viewport(const view_port& vp);

			bool set_render_target(size_t rt_index, surface_ptr rt_surface);
			bool set_depth_surface(surface_ptr depth_surface);

			surface_ptr get_render_target(size_t rt_index);
			surface_ptr get_depth_surface();

			device_impl& get_impl();
			const device_impl& get_impl() const;

		private:
			typedef std::list<device_object*> objects_list;
			typedef objects_list::iterator objects_iter;

			objects_iter register_obj(device_object* obj);
			void unregister_obj(objects_iter iter);

		private:
			objects_list m_objects;

			core::windows::handle m_hwnd;
			pimpl m_pimpl;
		};
	}
}