#pragma once

#include "base.h"

#include "rect.h"
#include "color.h"
#include "input.h"
#include "events.h"
#include "scriptobject.h"
#include "align.h"
#include "renderer.h"

#include <luabind/luabind.hpp>

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

namespace xml 
{
	class node;
};

namespace gui
{
	class System;
	class ScriptSystem;

	namespace events
	{
		struct NamedEvent;
		struct ClickEvent;
		struct TrackEvent;
		struct SizedEvent;
	}

	class WindowBase :
		public TreeNode<WindowBase>,
		public ScriptObject<WindowBase>,
		public events::sender
	{
	public:
		typedef WindowBase self_t;
		WindowBase(System& sys, const std::string& name = std::string());
		virtual ~WindowBase();

		std::string const& getName() const;
		void setName(const std::string& name);		

		static const char* GetType() { return "WindowBase"; }
		virtual const char* getType() const { return self_t::GetType(); }

		void suspendLayout() { m_suspended = true; onSuspendLayout(); }
		void resumeLayout() { m_suspended = false; onResumeLayout(); }

		void setArea(const Rect& rc);
		const Rect& getArea(void) const { return m_area; }

		void setPosition(const point& pt);
		const point getPosition(void) const { return m_area.getPosition(); }
		void setSize(const Size& sz);
		const Size getSize(void) const { return m_area.getSize(); }

		void setVisible(bool visible);
		bool getVisible() const { return m_visible; }

		void setEnabled(bool enable) { m_enabled = enable; }
		bool getEnabled() const { return m_enabled; }

		void setBackColor(Color cl)
		{
			m_backColor = cl;
			invalidate();
		}
		Color getBackColor() const { return m_backColor; }
		void setForeColor(Color cl)
		{
			m_foreColor = cl;
			invalidate();
		}
		Color getForeColor() const { return m_foreColor; }

		void setUserData(size_t ptr) { m_userData = ptr; }
		size_t getUserData() const { return m_userData; }

		void moveToFront(WindowBase* child);
		void bringToBack(WindowBase* child);
		void moveToFront();
		void bringToBack();
		virtual void rise();

		void setAlwaysOnTop(bool status) { m_alwaysOnTop = status; if(status) moveToFront(); }
		bool getAlwaysOnTop() const { return m_alwaysOnTop; }

		void setDisableRise(bool disable) { m_disableRise = disable; }
		bool getDisableRise() const { return m_disableRise; }

		virtual bool isCanHaveChildren(void) const { return true; }
		WindowBase* findChildWindow(const std::string& name);
		WindowBase const* getParent() const { return m_parent; }
		void addChildWindow(WindowBase* wnd);

		void setInputFocus(bool query);
		void resetInputFocus() { m_focus = false; }
		bool hasInputFocus() const { return m_focus; }
		bool isTabStop() const { return m_tabstop; }

		bool hitTest(const point& pt);
		bool isCursorInside() const;

		bool isDraggable() const { return m_draggable; }
		void setDraggable(bool drag) { m_draggable = drag;}
		bool isAcceptDrop() const { return m_acceptDrop; }
		void setAcceptDrop(bool drop) { m_acceptDrop = drop;}

		void setIgnoreInputEvents(bool ignore) { m_ignoreInputEvents = ignore;}
		bool getIgnoreInputEvents() const { return m_ignoreInputEvents; }

		virtual void invalidate();
		virtual void render(const Rect& dest, const Rect& clip) {clip;dest;}

		void draw(const point& offset, const Rect& clip);

		// events
		virtual bool onMouseEnter(void);
		virtual bool onMouseLeave(void);
		virtual bool onMouseMove(void);
		virtual bool onMouseWheel(int delta);
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onMouseDouble(EventArgs::MouseButtons btn);
		virtual bool onChar(const wchar_t* text);
		virtual bool onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state);
		virtual bool onCaptureGained(void);
		virtual bool onCaptureLost(void);
		virtual bool onLoad(void);
		virtual bool onSized(bool update = true);
		virtual bool onMoved(void);
		virtual bool onTick(float delta);
		virtual bool onTooltipShow();
		virtual bool onTooltipHide();
		virtual bool onSuspendLayout();
		virtual bool onResumeLayout();
		virtual bool onFocusGained();
		virtual bool onFocusLost(WindowBase* newFocus);

		// loading from XML
		virtual void init(xml::node& node);
		virtual void parseEventHandlers(xml::node& node);

		WindowBase* nextSibling();
		WindowBase* prevSibling();

		point transformToWndCoord(const point& global) const; // translate to parent coords!
		point transformToRootCoord(const point& local);

		void subscribeNamedEvent(std::string name, WindowBase* sender, luabind::object script_callback);
		void unsubscribeNamedEvent(std::string name, WindowBase* sender);
		void sendNamedEvent(std::string name);
		void onNamedEvent(events::NamedEvent& e);

		void addScriptEventHandler(std::string name, std::string handler);
		void callHandler(EventArgs* arg);

		void startTick(void);
		void stopTick(void);
		bool isUnsubscribePending() { if(m_unsubscribePending) {m_unsubscribePending = false; return true;} return false; }

		bool hasTooltip(void) const { return m_tooltip; }
		void enableTooltip(bool b) { m_tooltip = b; }

		std::string getEventScript(const std::string& ev);
		virtual bool onGameEvent(const std::string& ev);

		bool isChildrenOf(const WindowBase* wnd);
		
		void setAfterRenderCallback(AfterRenderCallbackFunc func)
		{
			m_afterRenderCallback = func;
		}

	protected:
		void executeScript(const std::string& env, const std::string& script);
		WindowBase& operator=(const WindowBase&) { return *this; }

		void align();
		void stick();

		void callAfterRenderCallback(const Rect& dest, const Rect& clip);

	protected:
		bool			m_suspended;

		Rect			m_area;
		Rect			m_alignmentRect;
		unsigned int	m_alignment;
		Rect			m_stickRect;
		unsigned int	m_stick;

		bool			m_visible;
		bool			m_enabled;
		Color			m_backColor;
		Color			m_foreColor;

		size_t			m_userData;

		bool			m_alwaysOnTop;
		bool			m_focus;
		bool			m_ignoreInputEvents;
		bool			m_tabstop;
		bool			m_draggable;
		bool			m_acceptDrop;
		bool			m_tooltip;
		bool			m_unsubscribePending;
		bool			m_customDraw;
		bool			m_invalidated;

		bool			m_disableRise;

		std::string		m_drawhandler;
		System&			m_system;
		AfterRenderCallbackFunc m_afterRenderCallback;

		typedef std::pair<std::string, WindowBase*> NamedEventEntry;
		typedef std::map<NamedEventEntry, luabind::object> NamedEventsMap;
		NamedEventsMap m_scriptevents;
		
		typedef std::unordered_map<std::string, std::string> HandlerMap;
		HandlerMap		m_handlers;

		std::string m_strName;
	};

	typedef std::shared_ptr<WindowBase> window_ptr;

	template <typename T> struct window_caster
	{
		static T* apply (WindowBase* w)
		{
			return dynamic_cast <T*> (w);
		}
	};
}

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif
