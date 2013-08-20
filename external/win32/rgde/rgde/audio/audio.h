#pragma once

#include <rgde/core/xml/xml.h>
#include <rgde/core/windows_forward.h>

namespace rgde
{
	namespace audio
	{
		namespace internal
		{
			class base_audio;
		}

		// Interface class to receive notifications that an audio
		// piece has finished playing 
		class listener
		{
		public:
			virtual void audio_started(internal::base_audio* p) {}; /*= 0;*/ //TODO:
			virtual void audio_finished(internal::base_audio* p) = 0;
		};

		typedef boost::shared_ptr<listener> listener_ptr;

		struct vec3
		{
			inline vec3(float fx=0, float fy=0, float fz=0)
				: x(fx), y(fy), z(fz) {}

			union{
				struct {float x, y, z;};
				float data[3];
			};
		};

		// A class representing the player's view camera, 
		// with the position orientation information exposed
		// for the audio_manager
		class camera
		{
		public:
			inline camera() {}
			inline virtual ~camera() {}

			virtual vec3& get_position() = 0;
			virtual vec3& get_front() = 0;
			virtual vec3& get_up() = 0;
		};


		// This class represents an object in the game world 
		// (with position and velocity). 3D sounds have corresponding
		// WorldObjects, so they know how the sound is moving in the game. 
		class world_object 
		{
		public:
			world_object();

			void get_position(float& x, float& y, float& z) const;
			const vec3& get_position() const;
			void set_position(float x, float y, float z);

			const vec3& get_velocity() const;
			void set_velocity(float x, float y, float z);

		protected:
			vec3 m_pos;
			vec3 m_vel;
		};

		typedef boost::shared_ptr<world_object> world_object_ptr;		


		class system
		{
		public:
			system(core::windows::handle handle);
			~system();

			void update(int ms_elapsed);

			void stop_all();
			void set_volume(int volume);

			bool load(xml::node node);
			bool load(const char* file);

			int get_num_tags();
			const char* get_tag_name(size_t tag_index);

			void set_camera(camera* cam);

			void play(const char* tag_name, int ms_duration = 0, int ms_delay = 0,	
				listener_ptr listener = listener_ptr(), world_object_ptr world_object  = world_object_ptr() );

			void play(size_t tag_index, int ms_duration = 0, int ms_delay = 0,	
				listener_ptr listener = listener_ptr(), world_object_ptr world_object  = world_object_ptr() );

		private:
			class audio_manager* manager;
		};
	}
}