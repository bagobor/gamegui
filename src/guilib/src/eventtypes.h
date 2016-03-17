#pragma once

namespace gui
{
	class WindowBase;
	namespace events
	{
		struct ResizeEvent {};
		struct ClickEvent {};
		struct TrackEvent {};
		struct SizedEvent {};
		struct MovedEvent {};

		struct NamedEvent
		{
			NamedEvent(const std::string& name, WindowBase* sender) 
				: m_name(name), m_sender(sender) {}

			std::string m_name;
			WindowBase* m_sender;
		};
	}
}
