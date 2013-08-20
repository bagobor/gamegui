#pragma once

#include <boost/noncopyable.hpp>

#include <rgde/core/types.h>

namespace rgde
{
	namespace core
	{
		namespace utils
		{
			class timer : boost::noncopyable
			{
			public:
				timer(bool start = true);

				void reset(); 				// resets the timer
				void start();				// starts the timer
				void stop();				// stop (or pause) the timer

				inline bool is_stopped() const {return m_is_stoped;}	// returns true if timer stopped

				double get_absolute_time(); // get the absolute system time
				double get_time();			// get the current time
								
				float get_elapsed_time();   // get the time between get_elapsed_time() calls

				// get all time values at once
				void get_time_values( double& time, double& absolute_time, float& elapsed_time );				

			protected:
				bool m_is_using_qpf;
				bool m_is_stoped;
				int64 m_ticks_per_second;

				int64 m_stop_time;
				int64 m_last_elapsed_time;
				int64 m_base_time;
			};
		}
	}
}