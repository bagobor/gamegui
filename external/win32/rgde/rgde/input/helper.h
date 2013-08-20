#pragma once

#include <rgde/input/base.h>

namespace rgde
{
namespace input
{
	class system;

	//helper

	//параметр для helper::handler_type
	struct helper_event
	{
		enum type
		{
			button,
			axis
		};

		type m_type;
		bool m_press;
		int  m_delta;
		int  m_time;
	};

	//обьект-посредник для получения информации о событиях ввода
	class helper
	{
	public:
		typedef boost::function<void(const helper_event&)> handler_type;

		virtual ~helper();

		void attach (const std::wstring &command_name);
		void detach ();

		void operator += (handler_type handler);

		command_ptr get_command() const {return m_command;}

	protected:
		explicit helper(system& system);
		helper (const std::wstring &command_name, system& system);

		friend class command;
		virtual void notify (const control &control);

	private:
		command_ptr         m_command;
		std::list<handler_type> m_handlers;
		system& m_system;
	};

	// button, обьект-посредник "кнопка"
	class button: public helper
	{
	public:
		typedef boost::function<void(bool)> button_handler_type;
		explicit button (system& system);
		button (const std::wstring &command_name, system& system);

		void operator += (button_handler_type handler);
		operator bool () const {return m_press > 0;}

	protected:
		friend class command;
		virtual void notify (const control &control);

	private:
		int m_press;
		std::list<button_handler_type> m_button_handlers;
	};

	// trigger, обьект-посредник "триггер"
	class trigger: public helper
	{
	public:
		typedef boost::function<void(bool)> trigger_handler_type;
		explicit trigger (system& system);
		trigger (const std::wstring &command_name, system& system);

		void operator += (trigger_handler_type handler);
		operator bool () const {return m_is_active;}

		void set_state (bool state);

	protected:
		virtual void notify (const control &control);

	private:
		bool m_is_active;
		std::list<trigger_handler_type> m_trigger_handlers;
	};

	// key_up, обьект-посредник "ОТжатие клавиши"
	class key_up: public helper
	{
	public:
		typedef boost::function<void()> keyup_handler_type;
		explicit key_up (system& system);
		key_up (const std::wstring &command_name, system& system);

		void operator += (keyup_handler_type handler);

	protected:
		friend class command;
		virtual void notify (const control &control);

	private:
		std::list<keyup_handler_type> m_keyup_handlers;
	};

	// key_down
	//обьект-посредник "НАжатие клавиши"
	class key_down: public helper
	{
	public:
		typedef boost::function<void()> keydown_handler_type;
		explicit key_down (system& system);
		key_down (const std::wstring &command_name, system& system);

		void operator += (keydown_handler_type handler);

	protected:
		friend class command;
		virtual void notify (const control &control);

	private:
		std::list<keydown_handler_type> m_keydownHandlers;
	};

	// relative_axis
	//обьект-посредник "относительная ось"
	class relative_axis : public helper
	{
	public:
		typedef boost::function<void(int)> relativeaxis_handle_type;
		explicit relative_axis (system& system);
		relative_axis (const std::wstring &command_name, system& system);

		void operator += (relativeaxis_handle_type handler);

	protected:
		friend class command;
		virtual void notify (const control &control);

	private:
		std::list<relativeaxis_handle_type> m_raxis_handlers;
	};

	// absolute_axis
	//обьект-посредник "абсолютная ось"
	class absolute_axis: public helper
	{
	public:
		typedef boost::function<void(int)> absolute_axis_handler_type;

		explicit absolute_axis (system& system);
		absolute_axis (const std::wstring &command_name, system& system);

		void operator += (absolute_axis_handler_type handler);
		operator int () const {return m_pos;}

		int  get_min () const {return m_min;}
		void set_min (int value);

		int  get_max () const {return m_max;}
		void set_max (int value);

		int  get_pos () const {return m_pos;}		
		void set_pos (int value);

	protected:
		friend class command;
		virtual void notify (const control &control);

	private:
		int m_min;
		int m_max;
		int m_pos;
		std::list<absolute_axis_handler_type> m_aaxis_handlers;
	};

	//// Cursor
	////обьект-посредник "курсор мыши"
	//class Cursor: public helper, public rgde::core::event::listener
	//{
	//public:
	//	typedef boost::function<void(float,float)> CursorHandler;

	//	Cursor ();

	//	void operator += (CursorHandler handler);

 //       float getX () const {return x;}
	//	float getY () const {return y;}
 //       void  set_pos (float x, float y);

	//protected:
 //       void onCursorMove (CCursorMove e);
 //       virtual void notify (const control &control);

 //       void adjustPosToWindow (float &x, float &y);
 //       void adjustPosToClient (float &x, float &y);

	//private:
 //       float x;
 //       float y;
	//	std::list<CursorHandler> m_cursorHandlers;
	//};

 //   // Mouse
	////обьект-посредник "мышь"
 //   class Mouse: public Cursor
 //   {
 //   public:
 //       enum ClickType
 //       {
 //           Down,
 //           Up,
 //           DoubleClick
 //       };

	//	typedef boost::function<void(int)>       WhellHandler;
	//	typedef boost::function<void(ClickType)> button_handler_type;

 //       Mouse ();

 //       void setMoveHandler         (CursorHandler handler);
	//	void setWhellHandler        (WhellHandler  handler);
 //       void setLeftButtonHandler   (button_handler_type handler);
 //       void setMiddleButtonHandler (button_handler_type handler);
 //       void setRightButtonHandler  (button_handler_type handler);

 //       bool isLeftPressed   () const {return m_left;}
 //       bool isMiddlePressed () const {return m_middle;}
 //       bool isRightPressed  () const {return m_right;}

 //   protected:
 //       void onWhell (CMouseWhell e);
 //       void onButton (CMouseButton e);

 //   private:
 //       bool m_left;
 //       bool m_middle;
 //       bool m_right;

 //   	std::list<WhellHandler> m_whellHandlers;
 //   	std::list<button_handler_type> m_leftButtonHandlers;
 //   	std::list<button_handler_type> m_middleButtonHandlers;
 //   	std::list<button_handler_type> m_rightButtonHandlers;
 //   };
}
}