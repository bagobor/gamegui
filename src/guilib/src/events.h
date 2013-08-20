#pragma once

namespace gui
{
namespace events
{
    class sender_base;
    class listener_base;
    class manager_base;

    // базовый класс менеджера событий
    class manager_base
    {
	protected:
		manager_base();
		virtual ~manager_base();

    public:
        //отписать получателя от менеджера (потомка класса manager_base)
		virtual void unsubscribe (listener_base*) = 0;
        virtual void unsubscribe (listener_base*, sender_base*) = 0;
    };

    // шаблонный класс менеджера событий
    template <typename Event>
    class manager: public manager_base
    {
        //подписка на получение событий
        struct subscription
        {
            listener_base                *m_pListener; //кто хочет получать события
            sender_base                  *m_pSender;   //от кого хочет получать события (0 - от всех)
            boost::function<void(Event)>  m_func;      //какой метод вызывать

            bool operator==(const subscription &s)
            {
                return m_pListener == s.m_pListener;
            }
        };

		 typedef std::list<subscription> SubscriptionsList;

    public:
        //синглтон
        static manager& get()
        {
            static manager instance;
            return instance;
        }

        //подписать l на получение событий в функтор func от отправителя pSender (если равен 0 - то от всех)
        void subscribe (listener_base *l, boost::function<void(Event)> func, sender_base *pSender)
        {
            subscription subs;
            subs.m_pListener = l;
            subs.m_func      = func;
            subs.m_pSender   = pSender;
            m_subscriptions.push_back(subs);
        }

		//отписать l от получения всех событий типа Event
		void unsubscribe (listener_base *listener)
		{
			SubscriptionsList::iterator i = m_subscriptions.begin();
			while (i != m_subscriptions.end())
			{
				if (i->m_pListener == listener)
					i = m_subscriptions.erase(i);
				else
					++i;
			}
		}

        //отписать l от получения всех событий типа Event
        void unsubscribe (listener_base *listener, sender_base *sender)
        {
            SubscriptionsList::iterator i = m_subscriptions.begin();
            while (i != m_subscriptions.end())
            {
                if (i->m_pListener == listener && i->m_pSender == sender)
                    i = m_subscriptions.erase(i);
                else
                    ++i;
            }
        }

        //отправить событие event от отправителя pSender
        void sendEvent (const Event& event, const sender_base *pSender)
        {
            SubscriptionsList tmp(m_subscriptions.begin(), m_subscriptions.end());

            SubscriptionsList::iterator i = tmp.begin();
            while (i != tmp.end())
            {
                if (std::find(m_subscriptions.begin(), m_subscriptions.end(), *i) != m_subscriptions.end())
                {
                    if (i->m_pSender == 0 || i->m_pSender == pSender)
                        i->m_func(event);
                    ++i;
                }
            }
        }

    private:
		manager () {}
		~manager() {}

        manager(const manager&);
        manager& operator= (const manager&);

        SubscriptionsList m_subscriptions;
    };


    // базовый класс получателя
    class  listener_base
    {
    protected:
        listener_base();
        virtual ~listener_base();

        //подписаться на получение событий
        template <typename Event>
        void subscribe( boost::function<void(Event)> f, sender_base *sender = 0)
        {
            manager<Event>::get().subscribe(this,f,sender);
        }

        //отписаться от получения событий
        template <typename Event>
        void unsubscribe(sender_base *sender)
        {
            manager<Event>::get().unsubscribe(this, sender);
        }

		template <typename Event>
		void unsubscribe()
		{
			manager<Event>::get().unsubscribe(this);
		}


    private:
        listener_base(const listener_base&);
        listener_base& operator= (const listener_base&);
    };

    // базовый класс отправителя
    class  sender_base
    {
    protected:
        sender_base();
		virtual ~sender_base();

        template<typename Event>
        void base_send_event(const Event& event)
        {
			manager<Event>::get().sendEvent(event,this);
        }

    private:
        sender_base(const sender_base&);
        sender_base& operator= (const sender_base&);
    };

	// получатель
	class  Listener: private listener_base
	{
	public:
		//подписаться на получение событий
		//  func    - объект с сигнатурой void(Event)
		//  pSender - от кого хотим получать события (0 или ничего, если хотим получать от всех)
		template <typename Event>
		void subscribe( boost::function<void(Event)> func, sender_base *pSender=0 )
		{
			listener_base::subscribe<Event>(func,pSender);
		}

		//подписаться на получение событий
		//  ptr     - указатель на член-функцию с сигнатурой void(Event)
		//  pSender - от кого хотим получать события (0 или ничего, если хотим получать от всех)
		template<typename Event, typename Class, typename EventArg>
		void subscribe (void (Class::*ptr)(EventArg), sender_base *pSender=0)
		{
			listener_base::subscribe<Event>( boost::bind(ptr, static_cast<Class*>(this), _1), pSender );
		}

		//отписаться от получения событий
		template <typename Event>
		void unsubscribe(sender_base* sender)
		{
			listener_base::unsubscribe<Event>(sender);
		}

		template <typename Event>
		void unsubscribe()
		{
			listener_base::unsubscribe<Event>();
		}
	};


	// отправитель
	class  Sender : public Listener, public sender_base
	{
	public:
		template<typename Event>
		void send_event(const Event& event)
		{
			base_send_event(event);
		}
	};
}//events
}//gui