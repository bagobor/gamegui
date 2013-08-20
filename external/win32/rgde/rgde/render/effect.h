#pragma once

#include <rgde/core/math.h>

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
		typedef boost::shared_ptr<class texture> texture_ptr;
		class device;

		namespace effects
		{
			typedef char* internal_effect_handle;

			typedef std::map<std::string, std::string> annotations_map;
			typedef annotations_map::iterator annot_iter;
			typedef annotations_map::const_iterator annot_const_iter;

			class base_handle
			{
				friend class effect;			
			public:
				base_handle();
				virtual ~base_handle();

				const std::string& get_name() const {return m_name;}

				virtual bool is_valid() const {return 0 != m_handle;}
				virtual void refresh(effect& e) = 0;

				bool is_annotation_exist(const std::string& annot_name) const;
				const std::string& get_annotation(const std::string& annot_name) const;
				const annotations_map& get_annotations() const {return annotations;}

			protected:
				std::string m_name;			
				internal_effect_handle m_handle;
				annotations_map annotations;
			};

			typedef boost::shared_ptr<base_handle> handle_ptr;
			typedef std::vector<handle_ptr> handles_vertor;

			class param_info : public base_handle
			{
				friend class effect;
			public:
				typedef std::vector<char> data;
				enum type
				{
					INT, STRING, FLOAT, FLOAT2, 
					FLOAT3, FLOAT4, TEXTURE, BOOL,
					INVALID
				};

				struct ui_params
				{
					ui_params();
					std::string name;
					bool has_slider;
					int slider_max;
					int slider_min;
					int slider_factor; //or Step
					bool is_color_swatch;
				};

				param_info() {m_is_tweakable = false;}

				/// Parameter tweakable in engine, FX Composer, RenderMonkey, etc??
				bool tweakable() const {return m_is_tweakable;}
				const std::string& get_semantic() const {return m_semantic;}

				virtual bool is_valid() const {return 0 != m_handle && m_is_used;}

				virtual void refresh(effect& e);	

				int get_columns() const {return m_columns;}
				type get_type() const {return m_type;}

				const data& get_data() const {return m_data;}

				const std::string& get_ui_name() const {return m_ui_params.name;}
				bool has_slider() const {return m_ui_params.has_slider;}
				const ui_params& get_ui_params() const {return m_ui_params;}
				const bool is_color_swatch() const {return m_ui_params.is_color_swatch;}

			protected:				
				data m_data;
				int m_columns;
				std::string m_semantic;
				bool m_is_used;
				bool m_is_tweakable;
				type m_type;
				ui_params m_ui_params;
			};

			typedef boost::shared_ptr<param_info> param_ptr;
			typedef std::map<std::string, param_ptr> params_map;
			typedef params_map::const_iterator params_iter;

			typedef std::vector<param_ptr> params_vector;

			/// Light types enumeration
			enum light_type
			{
				light_omni		= 0,
				light_spot		= (1<<1),
				light_dir		= (1<<2),
				light_omni_proj = (1<<3),
				light_max		= 0x7fffffff,
			};

			struct tech_info : public base_handle
			{
				friend class effect;

				tech_info();
				
				virtual void refresh(effect& e);			

				params_map params;			
				float ps_version;
				int light_types;
				int max_lights;
				bool per_pixel;
				bool shadow_caster;
				bool light_maping;
				bool supports_instancing;
				bool use_vs;
				bool override_engine_multipass;
				std::string group;

				// must stay same for shader reloading correct work
				int tech_index;
			};

			typedef boost::shared_ptr<tech_info> techinfo_ptr;

			typedef std::list<techinfo_ptr> tech_list;
			typedef tech_list::iterator tech_list_iter;
			typedef tech_list::const_iterator tech_list_citer;

			typedef std::map<std::string, techinfo_ptr> tech_map;
			typedef tech_map::iterator tech_map_iter;
			typedef tech_map::const_iterator tech_map_citer;

			class param_block
			{
				friend class effect;
				param_block(effect& parent_effect);
			public:			
				~param_block();

				void begin();
				void end();

				void apply();

				/// returns NULL if effect was killed.
				const effect* get_parent_effect() const {return m_parent_effect;}
				bool is_valid() const {return 0 != m_parent_effect && 0 != m_handle;}

			private:
				void free_handle();

			private:
				effect* m_parent_effect;
				internal_effect_handle m_handle;
			};

			typedef boost::shared_ptr<param_block> paramblock_ptr;
			typedef boost::shared_ptr<class effect> effect_ptr;

			class effect
			{
				friend param_info;
				friend tech_info;
				friend param_block;
				typedef boost::weak_ptr<param_block> paramblock_wptr;
				typedef std::list<paramblock_wptr> param_blocks;
				typedef param_blocks::iterator param_block_iter;

			public:
				effect(void* platform_handle, float shader_max_version = 2.2f);
				~effect();

				static effect_ptr create(device& dev, const void* data, size_t size);
				static effect_ptr create(device& dev, core::vfs::istream_ptr file);				

				techinfo_ptr get_tech(const std::string& tech_name);
				const tech_list& get_techniques() const {return m_techniques_list;}

				void set_tech(techinfo_ptr tech);

				void reload(void* new_platform_handle  = NULL);

				param_ptr get_param(const std::string& param_name) const;
				param_ptr get_param_by_semantic(const std::string& param_semantic) const;

				paramblock_ptr create_paramblock();

				bool set_tech(const std::string& tech_name);
				unsigned int begin(unsigned int flags);
				bool begin_pass(unsigned int pass);
				bool end_pass();
				bool end();

				bool set(const param_ptr& p, int v);
				bool set(const param_ptr& p, bool v);
				bool set(const param_ptr& p, float v);
				bool set(const param_ptr& p, texture_ptr tex);
				bool set(const param_ptr& p, const math::mat44f& v);
				bool set(const param_ptr& p, const math::vec4f& v);
				bool set(const param_ptr& p, const float* pf, unsigned int count);
				bool set(const param_ptr& p, const math::vec4f* vec_array, unsigned int count);
				bool set(const param_ptr& p, const math::mat44f* mat_array, unsigned int count);
				bool set(const param_ptr& p, void* data, unsigned int bytes);

				bool set(const std::string& param_name, int v);			
				bool set(const std::string& param_name, bool v);
				bool set(const std::string& param_name, float v);
				bool set(const std::string& param_name, texture_ptr tex);
				bool set(const std::string& param_name, const math::mat44f& v);
				bool set(const std::string& param_name, const math::vec4f& v);
				bool set(const std::string& param_name, const float* pf, unsigned int count);
				bool set(const std::string& param_name, const math::vec4f* pv, unsigned int count);
				bool set(const std::string& param_name, const math::mat44f* pm, unsigned int count);
				bool set(const std::string& param_name, void* data, unsigned int bytes);

				void commit_changes();

				void on_device_reset();
				void on_device_lost();

				const params_vector& GetParams() const {return m_params;}

				bool is_reads_color_buff() const {return m_reads_color_buffer;}
				bool is_reads_global_color_buff() const {return m_reads_global_color_buffer;}
				bool is_force_ldr() const {return m_force_ldr;}
				bool is_uses_light_array() const {return m_use_light_array;}

				static void skip_unused_params(bool val) {m_skip_unused_params = val;}

			private:
				void clean_param_blocks();
				void init(float shader_max_version);
				void parse_techniques(std::list<techinfo_ptr>& tech_list, float shader_max_version);
				void parse_params(std::list<param_ptr>& params_list);			
				void erase_param_block(param_block_iter iter);
				void fill_ui_params(std::list<param_ptr>& params_list);

			private:
				bool m_reads_color_buffer;
				bool m_reads_global_color_buffer;
				bool m_force_ldr;
				bool m_use_light_array;
				static bool m_skip_unused_params;

				params_vector m_params;
				params_map m_params_by_name;
				params_map m_params_by_semantic;
				param_blocks	m_param_blocks;
				
				tech_list m_techniques_list;
				tech_map m_techiques;
				handles_vertor m_handlers; // need to call refresh after reload

				void* m_platform_handle;
			};
		}
	}
}