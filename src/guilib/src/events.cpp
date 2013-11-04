#include "stdafx.h"

#include "events.h"

namespace gui
{
namespace events
{
	namespace
	{
		class list_managers
		{
		public:
			list_managers() {}
			~list_managers() {}

			void unsubscribe_all (details::base_listener *listener);
			void add_manager     (details::base_events_manager *manager);
			void remove_manager  (details::base_events_manager *manager);

		private:
			std::list<details::base_events_manager*> m_managers;
		} list_manager;
	}

	namespace details
	{
		base_events_manager::base_events_manager()
		{
			list_manager.add_manager(this);
		}

		base_events_manager::~base_events_manager()
		{
			list_manager.remove_manager(this);
		}

		base_listener::base_listener() {}

		base_listener::~base_listener()
		{		
			list_manager.unsubscribe_all(this);
		}

		base_sender::base_sender() {}

		base_sender::~base_sender() {}
	}

	void list_managers::unsubscribe_all(details::base_listener *listener)
	{
		std::list<details::base_events_manager*>::iterator i = m_managers.begin();
		for (; i != m_managers.end(); ++i) {
			(*i)->unsubscribe(listener);
		}
	}

	void list_managers::add_manager (details::base_events_manager *manager)
	{
		m_managers.push_back(manager);
	}

	void list_managers::remove_manager (details::base_events_manager *manager)
	{
		std::list<details::base_events_manager*>::iterator i = m_managers.begin();
		while (i != m_managers.end()) {
			if ((*i) == manager)
				i = m_managers.erase(i);
			else
				++i;
		}
	}
}
}