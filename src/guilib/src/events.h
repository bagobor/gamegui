#pragma once

#include <functional>

namespace gui
{
	namespace events
	{
		namespace details
		{
			class base_sender;
			class base_listener;
			class base_events_manager;

			class base_events_manager
			{
			protected:
				base_events_manager();
				virtual ~base_events_manager();

			public:
				virtual void unsubscribe(base_listener*) = 0;
				virtual void unsubscribe(base_listener*, void*) = 0;
			};
		}

		template <typename Event>
		class manager: public details::base_events_manager
		{
			struct subscription {
				details::base_listener *m_listener; 
				void *sender;
				std::function<void(Event)>  handler;

				bool operator==(const subscription &s) {
					return m_listener == s.m_listener;
				}
			};

			typedef std::list<subscription> subscriptions_list;
			typedef std::map<void*, subscriptions_list> subcriptions_map;

		public:
			static manager& get() {
				static manager instance;
				return instance;
			}

			//подписать listener на получение событий в функтор func от отправителя sender (если равен 0 - то от всех)
			void subscribe(details::base_listener *listener, std::function<void(Event)> func, void *sender) {
				subscription s = { listener, sender, func};
				
				// check for duplicates
				subscriptions_list &subs = m_subscriptions[sender];
				subscriptions_list::iterator i = subs.begin();
				for (; i != subs.end(); ++i) {
					if (i->m_listener == listener) {
						return;
					}
				}

				subs.push_back(s);
			}

			void unsubscribe(details::base_listener *listener) {
				subcriptions_map::iterator mi = m_subscriptions.begin();
				for (;mi != m_subscriptions.end(); ++mi) {
					subscriptions_list &subs = mi->second;
					subscriptions_list::iterator i = subs.begin();
					for (;i != subs.end();++i) {
						if (i->m_listener == listener) {
							i = subs.erase(i);
							break;
						}
					}
				}
			}

			void unsubscribe(details::base_listener *listener, void *sender)
			{
				subscriptions_list& concrete_subs = m_subscriptions[sender];
				subscriptions_list::iterator i = concrete_subs.begin();
				for (;i != concrete_subs.end(); ++i) {
					if (i->m_listener == listener) {
						i = concrete_subs.erase(i);
						break;
					}
				}
			}

			void send(const Event& event, void *sender)
			{
				subscriptions_list& broadcast_subs = m_subscriptions[0];
				if (!broadcast_subs.empty()) {
					subscriptions_list::iterator i = broadcast_subs.begin();
					for(;i != broadcast_subs.end();++i) {
						i->handler(event);
					}
				}
				subscriptions_list& concrete_subs = m_subscriptions[sender];
				if (!concrete_subs.empty()) {
					subscriptions_list::iterator i = concrete_subs.begin();
					for(;i != concrete_subs.end();++i) {
						i->handler(event);
					}
				}
			}

		private:
			manager () {}
			~manager() {}

			manager(const manager&);
			manager& operator= (const manager&);

			subcriptions_map m_subscriptions;
		};

		namespace details
		{
			class base_listener
			{
			protected:
				base_listener();
				virtual ~base_listener();

				template <typename Event>
				void subscribe( std::function<void(Event)> f, void *sender = 0) {
					manager<Event>::get().subscribe(this,f,sender);
				}

				template <typename Event>
				void unsubscribe(void *sender) {
					manager<Event>::get().unsubscribe(this, sender);
				}

				template <typename Event>
				void unsubscribe() {
					manager<Event>::get().unsubscribe(this);
				}

			private:
				base_listener(const base_listener&);
				base_listener& operator= (const base_listener&);
			};


			class base_sender
			{
			protected:
				base_sender();
				virtual ~base_sender();

				template<typename Event>
				void send(const Event& event) {
					manager<Event>::get().send(event, this);
				}

			private:
				base_sender(const base_sender&);
				base_sender& operator= (const base_sender&);
			};
		}

		struct listener	: private details::base_listener
		{
			template <typename Event>
			void subscribe( std::function<void(Event)> func, details::base_sender *sender=0 ) {
				details::base_listener::subscribe<Event>(func,sender);
			}

			template<typename Event, typename Class, typename EventArg>
			void subscribe (void (Class::*ptr)(EventArg), void *sender=0) {
				details::base_listener::subscribe<Event>( 
					std::bind(ptr, static_cast<Class*>(this), std::placeholders::_1), sender );
			}

			template <typename Event>
			void unsubscribe(details::base_sender* sender) {
				details::base_listener::unsubscribe<Event>(sender);
			}

			template <typename Event>
			void unsubscribe() {
				details::base_listener::unsubscribe<Event>();
			}
		};

		struct sender: public listener, public details::base_sender
		{
			//template<typename Event>
			//void send_event(const Event& event) {
			//	base_send(event);
			//}
		};
	}
}//gui