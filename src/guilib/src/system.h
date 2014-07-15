#pragma once

#include "cursor.h"
#include "input.h"
#include "scripthost.h"

namespace gui
{
	class base_window;
	typedef std::shared_ptr<base_window> window_ptr;
	class RenderHelper;
	class DragContainer;
	class Tooltip;
	class Menu;
	class WindowManager;
	class Size;

	enum sys_keys {
		SHIFT	=	1,
		RSHIFT	=	2,
		CONTROL	=	4,
		RCONTROL =	8,
		MENU	=	16,
		RMENU	=	32
	};

	struct log {
		enum level	{
			system = 0,
			message = 1,
			warning = 2,
			error = 3,
			critical = 4,
			log_levels_num = 5
		};

		void wrn(const std::string& text) { write(warning, text); }
		void msg(const std::string& text) { write(warning, text); }
		void err(const std::string& text) { write(error, text); }
		void crit(const std::string& text) { write(critical, text); }

		virtual void write(level l, const std::string& msg) {l;msg;}
	};

	extern log s_null_log;

	struct data {
		void* ptr;
		size_t size;

		~data() {delete [] ptr; ptr = 0;}
	};

	typedef std::shared_ptr<data> data_ptr;

	struct filesystem{
		virtual ~filesystem() {}
		/// returns zero terminated string
		virtual std::string load_text(const std::string& filename) = 0;
		virtual data_ptr load_binary(const std::string& filename) = 0;
	};
	typedef std::shared_ptr<filesystem> filesystem_ptr;

	/// event type flags
	enum event_type {
		event_mouse = 0,
		event_keyboard = 2, 
		event_char = 4, 
		event_focus = 8,
		event_viewport_resize = 16,

		event_type_filter = event_mouse + event_keyboard + event_char + event_focus + event_viewport_resize,

		mouse_move = 256, mouse_wheel=512, mouse_button=1024, mouse_dbclick=2048,
		mouse_event_filter = mouse_move + mouse_wheel + mouse_button + mouse_dbclick,

		event_key_down = 32, event_key_up = 64,
	};

	enum {button_left=0, button_middle=1, button_right=2};

	union event{
		//event_type type;
		int type;
		struct {
			int type;
			int x, y;
			int delta;			
			int button;
		} mouse;
		struct {
			int type;
			EventArgs::Keys key;
		} keyboard;
		struct {
			int type;
			unsigned int code;
		} text;
		struct {
			int type;
		} focus;
	};

	class  System
	{
	public:
		System(Renderer& render, const std::string& scheme,
			lua_State* externalLua = 0, log& l = s_null_log);

		~System(void);

		void reset();
		void reset(bool complete);

		// public interface
		bool handle_event(event e);

		bool isSysKeyPressed(unsigned int key);
		size_t GetSystemKeys() { return m_sytemkeys; }

		bool isInputCaptured() { return m_captureWindow != 0; }

		Cursor& getCursor() { return m_cursor; }
		const Cursor& getCursorConst() const { return m_cursor; }
		void setShowCursor(bool visible);
		
		void logEvent(log::level level, const std::string& message);

		base_window* find(const std::string& name);
		
		void render();

		void init();

		void tick(float delta);
		void subscribeTick(base_window* wnd);
		void unsubscribeTick(base_window* wnd);
		void setTickClear() { m_tickClear = true; }

		void draw();

		base_window&	getRootWindow() const { return *(m_rootWindow.get()); }
		window_ptr		getRootPtr();

		base_window*	getWindowWithFocus() const { return m_focusWindow; }
		base_window*	getCaptureWindow() const { return m_captureWindow; }
		bool			queryInputFocus(base_window* wnd);
		void			queryCaptureInput(base_window* wnd);

		void			EnterExclusiveInputMode(base_window* wnd);
		void			LeaveExclusiveInputMode();

		Tooltip*		getTooltip() const;
		void			setTooltipDelay(float f)	{ m_ttdelay = f; }
		void			setTooltipLifetime(float f)	{ m_ttlifetime = f; }
		float			getTooltipDelay() const		{ return m_ttdelay; }
		float			getTooltipLifetime() const	{ return m_ttlifetime; }

		void			showTooltip(base_window* wnd);
		void			hideTooltip(base_window* wnd);

		DragContainer*	getDragContainer() const;
		bool			startDrag(base_window* wnd, point offset);
		bool			stopDrag(void);
		void			freezeDrag() { m_dragfreeze = true; }
		void			unfreezeDrag() { m_dragfreeze = false; }
		bool			isDragFrozen() const { return m_dragfreeze; }

		Menu*			getMenu(void) const;
		void			setContextMenuState(bool s) { m_isContextMenu = s; }

		Renderer&		getRenderer() { return m_render; }
		WindowManager&	getWindowManager() { return *m_windowMgr; }

		void setAutoScale(bool status);

		void			executeScript(const std::string& filename);	
		void			executeScript(const std::string& filename, base_window* wnd);
		
		base_window*		createWindow(base_window* parent, const std::string& name, const std::string& type_or_path);
		base_window*		loadXml(base_window& parent, const std::string& filename);
		base_window*		loadXml(const std::string& filename);
		base_window*		getTargetWindow(const point& pt) const;

		ScriptSystem&	getScriptSystem() { return m_scriptSys; }

		bool			isMouseInGui() const;
		bool			isMouseInGui(float x, float y) const;

		filesystem_ptr filesystem() { return m_filesystem; }

	protected:
		bool		proceedSystemKey(EventArgs::Keys key, EventArgs::ButtonState state);
		window_ptr	loadXml_(const std::string& filename);

		void		makeLuaBinding(void);
		void		updateSize(Size& sz);

		bool handleMouseMove(int x, int y);
		bool handleMouseWheel(int diff);
		bool handleMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		bool handleMouseDouble(EventArgs::MouseButtons btn);
		bool handleChar(unsigned int code);
		bool handleKeyboard(EventArgs::Keys key, EventArgs::ButtonState state);
		void handleFocusLost(void);

		void handleViewportChange();

	protected:
		Renderer&		m_render;
		std::unique_ptr<WindowManager>	m_windowMgr;
		std::unique_ptr<RenderHelper>	m_renderHelper;

		filesystem_ptr m_filesystem;

		ScriptSystem	m_scriptSys;

		bool m_autoScale;

		Cursor		m_cursor;
		base_window*	m_exclusiveInputWindow;

		window_ptr		m_rootWindow;
		base_window*	m_focusWindow;
		base_window*	m_captureWindow;
		base_window*	m_containsMouse;
		window_ptr		m_dragContainer;
		window_ptr		m_tooltipWindow;
		window_ptr		m_menuWindow;

		bool			m_isContextMenu;
		
		float			m_ttdelay;
		float			m_ttlifetime;
		float			m_tttime;
		bool			m_activateTooltip;

		bool	m_dragging;
		bool	m_dragfreeze;
		bool	m_dragfired;
		point	m_dragOffset;
		float	m_dragThreshold;
		base_window*	m_dragWindow;

		size_t			m_sytemkeys;

		log&	m_logger;

		std::vector<base_window*>	m_tickedWnd;
		std::vector<base_window*>	m_subscribeTickWnd;
		bool						m_inTick;
		bool						m_tickClear;
	};
}
