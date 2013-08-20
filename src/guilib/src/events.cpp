//EventBase.cpp
#include "stdafx.h"
#include "events.h"

namespace gui
{
namespace events
{
	namespace
	{
		class CListManagers
		{
		public:
			CListManagers() {}
			~CListManagers() {}

			//отписать данного получателя ото всех менеджеров
			void unsubscribeAll (listener_base *l);
			//менеджер добавляет себя в общий список менеджеров
			void addManager     (manager_base *m);
			//менеджер удаляет себя из общего списка менеджеров
			void delManager     (manager_base *m);

		private:
			std::list<manager_base*> m_managers;
		} list_manager;
	}


	manager_base::manager_base()
	{
		list_manager.addManager(this);
	}

	manager_base::~manager_base()
	{
		list_manager.delManager(this);
	}

	listener_base::listener_base() 
	{
	}

	//отписать получателя от всех менеджеров
	listener_base::~listener_base()
	{		
		list_manager.unsubscribeAll(this);
	}

	sender_base::sender_base() 
	{
	}

	sender_base::~sender_base()
	{
	}

    //отписать данного получателя ото всех менеджеров
    void CListManagers::unsubscribeAll(listener_base *l)
    {
        std::list<manager_base*>::iterator i = m_managers.begin();
        while (i != m_managers.end())
        {
            (*i)->unsubscribe(l);
            ++i;
        }
    }

    //менеджер добавляет себя в общий список менеджеров
    void CListManagers::addManager (manager_base *m)
    {
        m_managers.push_back(m);
    }

    //менеджер удаляет себя из общего списка менеджеров
    void CListManagers::delManager (manager_base *m)
    {
        std::list<manager_base*>::iterator i = m_managers.begin();
        while (i != m_managers.end())
        {
            if ((*i) == m)
                i = m_managers.erase(i);
            else
                ++i;
        }
    }

}//events
}//gui