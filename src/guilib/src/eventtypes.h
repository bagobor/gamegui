#pragma once

namespace gui
{
	class base_window;
	namespace events
	{
		struct ResizeEvent {};
		struct ClickEvent {};
		struct TrackEvent {};
		struct SizedEvent {};
		struct MovedEvent {};

		struct NamedEvent
		{
			NamedEvent(const std::string& name, base_window* sender) 
				: m_name(name), m_sender(sender) {}

			std::string m_name;
			base_window* m_sender;
		};
	}
}
