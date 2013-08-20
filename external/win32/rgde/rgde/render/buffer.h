#pragma  once 
#include <rgde/render/resource.h>

namespace rgde
{
	namespace render
	{
		class buffer : public resource
		{
		public:
			enum usage_flags
			{
				write_only = 1,
				dynamic	   = 2
			};

			enum lock_flags
			{
				readonly           = 1,
				discard            = 2,
				nooverwrite        = 4,
				nosyslock          = 8,
				donotwait          = 16,
				no_dirty_update    = 32
			};

			bool is_dynamic() const {return (m_buffer_usage & dynamic) > 0;}
			bool is_write_only() const {return (m_buffer_usage & write_only) > 0;}

			// need for later replacement such as read\write methods (for safety and simplicity).
			virtual void* lock(uint offset_to_lock, uint size_to_lock, ulong flags) = 0;
			virtual void unlock() = 0;

		protected:
			buffer(device& dev, resource::type type, resource::format format, 
					resource::pool pool, uint buffer_usage, bool register_on_lost_reset);

		protected:
			uint m_buffer_usage;
		};

	
		// Vertex shader declaration
		// Vertex element semantics
		struct vertex_element
		{
			enum decl_usage
			{
				position = 0,
				blend_weight = 1,  // 1
				blend_indices = 2, // 2
				normal = 3,        // 3
				psize = 4,         // 4
				texcoord = 5,      // 5
				tangent = 6,       // 6
				binormal = 7,      // 7
				positiont = 9,     // 9
				color = 10,         // 10
				fog = 11,           // 11
				depth = 12,         // 12
				decl_usage_max_value = 0xFF
			};

			enum decl_method
			{
				default_method = 0,
				decl_method_max_value = 0xFF
			};

			// Declarations for type fields
			enum element_type
			{
				float1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
				float2    =  1,  // 2D float expanded to (value, value, 0., 1.)
				float3    =  2,  // 3D float expanded to (value, value, value, 1.)
				float4    =  3,  // 4D float
				color4ub  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. range
								 // Input is in color4ub format (ARGB) expanded to (R, G, B, A)
				ubyte4    =  5,  // 4D unsigned byte
				short2    =  6,  // 2D signed short expanded to (value, value, 0., 1.)
				short4    =  7,  // 4D signed short
				unused    = 17,  // When the type field in a decl is unused.
				element_type_max_value = 0xFF
			};

			ushort			stream;      // Stream index
			ushort			offset;      // Offset in the stream in bytes
			element_type    type;        // Data type
			decl_method		method;      // Processing method
			decl_usage		usage;       // Semantics
			byte			usage_index; // Semantic index

			const static vertex_element end_element;
		};

		typedef std::vector<vertex_element> vertex_elements_vector;

		class vertex_declaration
		{
		public:
			class vertex_declaration_impl;
			typedef boost::shared_ptr<vertex_declaration_impl> pimpl;

		private:
			vertex_declaration(device& dev, const vertex_element* vertex_elements_array, size_t size);
			vertex_declaration(device& dev, const vertex_elements_vector& vertex_elements);

		public:
			static vertex_declaration_ptr create(device& dev, const vertex_element* vertex_elements_array, size_t num_elements);
			static vertex_declaration_ptr create(device& dev, const vertex_elements_vector& vertex_elements);

			vertex_declaration_impl& get_impl();
			const vertex_declaration_impl& get_impl() const;
			
			const vertex_elements_vector& get_vertex_elemets() const {return m_vertex_elements;}

		private:
			vertex_elements_vector m_vertex_elements;
			pimpl m_impl;
		};

		class vertex_buffer : public buffer
		{
			class vertex_buffer_impl;
			typedef boost::shared_ptr<vertex_buffer_impl> pimpl;

			vertex_buffer(device& dev, vertex_declaration_ptr decl, size_t size, resource::pool pool, uint usage_flags);
		public:
			static vertex_buffer_ptr create(device& dev, vertex_declaration_ptr decl, 
								size_t size, resource::pool pool, uint usage_flags = 0);
			
			~vertex_buffer();

			void* lock(uint offset_to_lock, uint size_to_lock, ulong flags);
			void unlock();

			vertex_declaration_ptr get_declaration()  const {return m_decl;}

			vertex_buffer_impl& get_impl();
			const vertex_buffer_impl& get_impl() const;

		private:
			vertex_declaration_ptr m_decl;
			pimpl m_pimpl;
		};

		class index_buffer : public buffer
		{
			class index_buffer_impl;
			typedef boost::shared_ptr<index_buffer_impl> pimpl;

		protected:
			index_buffer(device& dev, size_t size, bool use_32bit, resource::pool pool, 
						uint usage_flags = 0);

		public:
			static index_buffer_ptr create(device& dev, size_t size, bool use_32bit, 
											resource::pool pool, uint usage_flags = buffer::write_only);
			~index_buffer();

			void* lock(uint offset_to_lock, uint size_to_lock, ulong flags);
			void unlock();

			index_buffer_impl& get_impl();
			const index_buffer_impl& get_impl() const;

		private:
			pimpl m_pimpl;
		};
	}
}