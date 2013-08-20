#pragma once

namespace rgde
{
	namespace core
	{
		namespace events
		{
			namespace details
			{
				class base_sender;
				class base_listener;
				class base_events_manager;

				// базовый класс менеджера событий
				class base_events_manager
				{
				protected:
					base_events_manager();
					virtual ~base_events_manager();

				public:
					//отписать получателя от менеджера (потомка класса base_events_manager)
					virtual void unsubscribe (base_listener*) = 0;
					virtual void unsubscribe (base_listener*, base_sender*) = 0;
				};
			}

			// шаблонный класс менеджера событий
			template <typename Event>
			class manager: public details::base_events_manager
			{
				//подписка на получение событий
				struct subscription
				{
					details::base_listener                *m_listener; //кто хочет получать события
					details::base_sender                  *m_sender;   //от кого хочет получать события (0 - от всех)
					boost::function<void(Event)>  m_func;      //какой метод вызывать

					bool operator==(const subscription &s)
					{
						return m_listener == s.m_listener;
					}
				};

				 typedef std::list<subscription> subscriptions_list;

			public:
				//синглтон
				static manager& get()
				{
					static manager instance;
					return instance;
				}

				//подписать listener на получение событий в функтор func от отправителя sender (если равен 0 - то от всех)
				void subscribe (details::base_listener *listener, boost::function<void(Event)> func, 
					details::base_sender *sender)
				{
					subscription subs;
					subs.m_listener = listener;
					subs.m_func      = func;
					subs.m_sender   = sender;
					m_subscriptions.push_back(subs);
				}

				//отписать listener от получения всех событий типа Event
				void unsubscribe (details::base_listener *listener)
				{
					subscriptions_list::iterator i = m_subscriptions.begin();
					while (i != m_subscriptions.end())
					{
						if (i->m_listener == listener)
							i = m_subscriptions.erase(i);
						else
							++i;
					}
				}

				//отписать listener от получения всех событий типа Event
				void unsubscribe (details::base_listener *listener, details::base_sender *sender)
				{
					subscriptions_list::iterator i = m_subscriptions.begin();
					while (i != m_subscriptions.end())
					{
						if (i->m_listener == listener && i->m_sender == sender)
							i = m_subscriptions.erase(i);
						else
							++i;
					}
				}

				//отправить событие event от отправителя sender
				void send_event (const Event& event, const details::base_sender *sender)
				{
					subscriptions_list tmp(m_subscriptions.begin(), m_subscriptions.end());

					subscriptions_list::iterator i = tmp.begin();
					while (i != tmp.end())
					{
						if (std::find(m_subscriptions.begin(), m_subscriptions.end(), *i) != m_subscriptions.end())
						{
							if (i->m_sender == 0 || i->m_sender == sender)
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

				subscriptions_list m_subscriptions;
			};

			namespace details
			{
				// базовый класс получателя
				class base_listener
				{
				protected:
					base_listener();
					virtual ~base_listener();

					//подписаться на получение событий
					template <typename Event>
					void subscribe( boost::function<void(Event)> f, base_sender *sender = 0)
					{
						manager<Event>::get().subscribe(this,f,sender);
					}

					//отписаться от получения событий
					template <typename Event>
					void unsubscribe(base_sender *sender)
					{
						manager<Event>::get().unsubscribe(this, sender);
					}

					template <typename Event>
					void unsubscribe()
					{
						manager<Event>::get().unsubscribe(this);
					}


				private:
					base_listener(const base_listener&);
					base_listener& operator= (const base_listener&);
				};


				// базовый класс отправителя
				class base_sender
				{
				protected:
					base_sender();
					virtual ~base_sender();

					template<typename Event>
					void base_send_event(const Event& event)
					{
						manager<Event>::get().send_event(event,this);
					}

				private:
					base_sender(const base_sender&);
					base_sender& operator= (const base_sender&);
				};
			}


			// получатель
			class listener	: private details::base_listener
			{
			public:
				//подписаться на получение событий
				//  func    - объект с сигнатурой void(Event)
				//  sender - от кого хотим получать события (0 или ничего, если хотим получать от всех)
				template <typename Event>
				void subscribe( boost::function<void(Event)> func, details::base_sender *sender=0 )
				{
					details::base_listener::subscribe<Event>(func,sender);
				}

				//подписаться на получение событий
				//  ptr     - указатель на член-функцию с сигнатурой void(Event)
				//  sender - от кого хотим получать события (0 или ничего, если хотим получать от всех)
				template<typename Event, typename Class, typename EventArg>
				void subscribe (void (Class::*ptr)(EventArg), details::base_sender *sender=0)
				{
					details::base_listener::subscribe<Event>( 
						boost::bind(ptr, static_cast<Class*>(this), _1), sender );
				}

				//отписаться от получения событий
				template <typename Event>
				void unsubscribe(details::base_sender* sender)
				{
					details::base_listener::unsubscribe<Event>(sender);
				}

				template <typename Event>
				void unsubscribe()
				{
					details::base_listener::unsubscribe<Event>();
				}
			};



			// отправитель
			class sender: public listener, public details::base_sender
			{
			public:
				template<typename Event>
				void send_event(const Event& event)
				{
					base_send_event(event);
				}
			};
		}
	}
}