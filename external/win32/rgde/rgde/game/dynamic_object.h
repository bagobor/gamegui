#pragma once

namespace game
{
	//динамический обьект для игры
	//(у него вызывается метод update из game_task)
	class dynamic_object
	{
		friend class game_system;
	public:
		dynamic_object();
		virtual ~dynamic_object();
		virtual void update(float dt)=0;

	private:
		void unsubscribe() {m_is_subscribed = false;}

	private:
		bool m_is_subscribed;
	};
}