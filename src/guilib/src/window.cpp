#include "stdafx.h"
#include "window.h"

#include "utils.h"
#include "system.h"

#include "eventtypes.h"

#include "../xml/xml.h"

namespace gui
{

base_window::base_window(System& sys, const std::string& name) : 
	named_object(name),
	m_system(sys),
	m_visible(false),
	m_enabled(true),
	m_area(0.f, 0.f, 1.f, 1.f),
	m_userData(0),
	m_focus(false),
	m_tabstop(false),
	m_ignoreInputEvents(false),
	m_alwaysOnTop(false),
	m_draggable(false),
	m_acceptDrop(false),
	m_customDraw(false),
	m_tooltip(false),
	m_unsubscribePending(false),
	m_alignmentRect(0.f, 0.f, 0.f, 0.f),
	m_invalidated(true),
	m_alignment(0),
	m_stick(0),
	m_stickRect(0.f, 0.f, 0.f, 0.f),
	m_afterRenderCallback(nullptr),
	m_suspended(false),
	ScriptObject<base_window>(sys.getScriptSystem())
{
	m_backColor = Color(1.f, 1.f, 1.f);
	m_foreColor = Color(0.f, 0.f, 0.f);
}

base_window::~base_window()
{
	m_system.getRenderer().clearCache(this);
}

void base_window::setArea(const Rect& rc) 
{ 
	m_area = rc;
	onMoved();
	onSized();
}

void base_window::setPosition(const point& pt) 
{
	if(pt != m_area.getPosition())
	{
		invalidate();
		m_area.setPosition(pt);
		onMoved();
		send_event(events::MovedEvent());
	}
}

void base_window::setSize(const Size& sz) 
{
	if(sz != m_area.getSize())
	{
		invalidate();
		m_area.setSize(sz);
		onSized();
		send_event(events::SizedEvent());
	}
}

bool base_window::isCursorInside() const {
	point pt = transformToWndCoord(m_system.getCursor().getPosition());
	return m_area.isPointInRect(pt);
}

void base_window::invalidate()	
{
	//m_system.getRenderer().clearCache(this);

	m_invalidated = true;	
	child_iter i = m_children.begin();
	child_iter end = m_children.end();
	while(i != end)
	{
		(*i)->invalidate();
		++i;
	}
}

void base_window::setVisible(bool visible) 
{
	EventArgs a;
	a.name = visible ? "On_Show" : "On_Hide";
	callHandler(&a);
	m_visible = visible;
}

void base_window::setInputFocus(bool query)
{
	m_focus = query ? m_system.queryInputFocus(this) : true;
}

bool base_window::hitTest(const point& pt)
{
	if(!m_visible)
		return false;

	if(!m_ignoreInputEvents)
		return m_area.isPointInRect(transformToWndCoord(pt));
	
	return false;
}

void base_window::rise()
{
	if(m_parent)
	{
		m_parent->rise();
	}
}

namespace
{
	struct seeker
	{
		const base_window* m_ptr;
		seeker(const base_window* ptr) : m_ptr(ptr){}
		bool operator()(window_ptr obj) 
		{
			return obj ? (obj.get() == m_ptr) : false;
		}
	};
}

struct topmost_{
	bool operator()(base_window::node_ptr obj)
	{
		return obj->getAlwaysOnTop();
	}
};
struct ntopmost_{
	bool operator()(base_window::node_ptr obj)
	{
		return !obj->getAlwaysOnTop();
	}
};

void base_window::moveToFront(base_window* child)
{
	if(m_children.size() <= 1)
		return;

	child_iter it = std::find_if(m_children.begin(), m_children.end(), seeker(child));
	if(it != m_children.end())
	{
		if(child->getAlwaysOnTop())
		{
			m_children.splice(m_children.end(), m_children, it);
		}
		else
		{
			child_iter topmost = std::find_if(m_children.begin(), m_children.end(), topmost_());
			m_children.splice(topmost, m_children, it);
		}
	}
}

void base_window::bringToBack(base_window* child)
{
	if(m_children.size() <= 1)
		return;

	child_iter it = std::find_if(m_children.begin(), m_children.end(), seeker(child));
	if(it != m_children.end())
	{
		if(child->getAlwaysOnTop())
		{
			child_iter topmost = std::find_if(m_children.begin(), m_children.end(), ntopmost_());
			m_children.splice(topmost, m_children, it);
		}
		else
		{
			m_children.splice(m_children.begin(), m_children, it);
		}
	}
}

void base_window::moveToFront()
{
	if(m_parent)
		m_parent->moveToFront(this);	
}

void base_window::bringToBack()
{
	if(m_parent)
		m_parent->bringToBack(this);
}

base_window* base_window::findChildWindow(const std::string& name)
{
	window_ptr p = find(name);
	return p.get();
}

void base_window::addChildWindow(base_window* wnd)
{
	if(wnd) add(window_ptr(wnd));
}

void base_window::callHandler(EventArgs* arg)
{
	if(!arg) return;

	HandlerMap::iterator it = m_handlers.find(arg->name);
	if (it == m_handlers.end()) return;

	std::string& handler = it->second;
	if (handler.empty()) return;

	luabind::object globals = luabind::globals(m_system.getScriptSystem().getLuaState());

	globals["eventArgs"] = arg;
	ExecuteScript(arg->name, handler);
	globals["eventArgs"] = 0;
}

void base_window::ExecuteScript(const std::string& env, const std::string& script)
{
	if(!m_system.getScriptSystem().ExecuteString(script, this, env))
	{
		m_system.logEvent(log::error, std::string("Unable to execute Lua handler '")+ env + std::string("' in object ") + getName());
		m_system.logEvent(log::error, m_system.getScriptSystem().GetLastError());
	}
}


bool base_window::onMouseEnter(void)
{
	MouseEventArgs m;
	m.name = "On_MouseEnter";	
	callHandler(&m);
	return m.handled; 
}
bool base_window::onMouseLeave(void)
{
	MouseEventArgs m;
	m.name = "On_MouseLeave";
	callHandler(&m);
	return m.handled; 
}
bool base_window::onMouseMove(void)
{
	MouseEventArgs m;
	m.name = "On_MouseMove";
	point pt = m_system.getCursor().getPosition();
	m.mouseX = pt.x;
	m.mouseY = pt.y;
	callHandler(&m);
	return m.handled; 
}
bool base_window::onMouseWheel(int delta)
{
	MouseEventArgs m;
	m.name = "On_MouseWheel";
	m.wheelDelta = delta;
	callHandler(&m);
	return m.handled; 
}
bool base_window::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
{
	MouseEventArgs m;
	m.name = "On_MouseButton";
	m.mbutton = btn;
	m.mstate = state;
	callHandler(&m);
	return m.handled; 
}
bool base_window::onMouseDouble(EventArgs::MouseButtons btn)
{
	MouseEventArgs m;
	m.name = "On_MouseDouble";
	m.mbutton = btn;
	callHandler(&m);
	return m.handled;
}
bool base_window::onChar(const wchar_t* text)
{
	KeyEventArgs k;
	k.name = "On_Char";
	k.ch = text;
	callHandler(&k);
	return k.handled; 
}
bool base_window::onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state)
{
	KeyEventArgs k;
	k.name = "On_KeyboardButton";
	k.button = key;
	k.state = state;
	callHandler(&k);
	return k.handled; 
}
bool base_window::onCaptureGained(void)
{
	EventArgs a;
	a.name = "On_CaptureGained";
	callHandler(&a);
	return a.handled; 
}
bool base_window::onCaptureLost(void)
{
	EventArgs a;
	a.name = "On_CaptureLost";
	callHandler(&a);
	return a.handled; 
}

bool base_window::onSuspendLayout(void)
{
	EventArgs a;
	a.name = "On_SuspendLayout";
	callHandler(&a);
	return a.handled; 
}
bool base_window::onResumeLayout(void)
{
	EventArgs a;
	a.name = "On_ResumeLayout";
	callHandler(&a);
	return a.handled; 
}

bool base_window::onFocusGained(void)
{
	EventArgs a;
	a.name = "On_FocusGained";
	callHandler(&a);
	return a.handled; 
}
bool base_window::onFocusLost(base_window* newFocus)
{
	EventArgs a;
	a.name = "On_FocusLost";
	callHandler(&a);
	return a.handled; 
}

bool base_window::onLoad(void)
{
	onMoved();
	if(m_alwaysOnTop)
		moveToFront();

	HandlerMap::iterator it = m_handlers.find("On_Draw");
	if(it != m_handlers.end())
	{
		m_drawhandler = it->second;
		if(!m_drawhandler.empty())
		{
			m_customDraw = true;
		}
	}

	EventArgs a;
	a.name = "On_Load";
	callHandler(&a);
	return a.handled; 
}

bool base_window::onSized(bool update)
{
	if(update)
	{
		Align();
		if(m_alignment)
			onMoved();
	}

	child_iter i = m_children.begin();
	child_iter end = m_children.end();
	while(i != end)
	{
		(*i)->onSized();
		++i;
	}
	EventArgs a;
	a.name = "On_Sized";
	callHandler(&a);

	return true;
}

void base_window::Align()
{
	if(!m_parent)
		return;
	if(!m_alignment)
		return;

	point pos = m_area.getPosition();
	Size sz = m_area.getSize();
	Size parent_size(m_parent->getSize());

	if(m_alignment & HCenter)
	{
		float original_width = m_alignmentRect.m_left + m_alignmentRect.m_right;
		float x = 0.f;
		if(original_width > 0.f)
		{
			float width = parent_size.width - sz.width;
			x = width * m_alignmentRect.m_left / original_width;
		}
		m_area.setPosition(point(x, pos.y));
	}
	else
	{
		if(m_alignment & Left)
		{
			m_area.setPosition(point(m_alignmentRect.m_left, pos.y));
		}
		else if(m_alignment & Right)
		{
			m_area.setPosition(point(parent_size.width - sz.width - m_alignmentRect.m_right, pos.y));
		}
	}

	pos = m_area.getPosition();

	if(m_alignment & VCenter)
	{
		float original_height = m_alignmentRect.m_top + m_alignmentRect.m_bottom;
		float y = 0.f;
		if(original_height > 0.f)
		{
			float height = parent_size.height - sz.height;
			y = height * m_alignmentRect.m_top / original_height;
		}
		m_area.setPosition(point(pos.x, y));
	}
	else
	{
		if(m_alignment & Top)
		{
			m_area.setPosition(point(pos.x, m_alignmentRect.m_top));
		}
		else if(m_alignment & Bottom)
		{
			m_area.setPosition(point(pos.x, parent_size.height - sz.height - m_alignmentRect.m_bottom));
		}
	}
}

void base_window::Stick()
{
	if(!m_parent)
		return;
	if(!m_stick)
		return;

	Size sz = m_area.getSize();
	Size parent_size(m_parent->getSize());

	if(m_stick & HCenter)
	{
		m_area.setPosition(point((parent_size.width - sz.width) / 2, m_area.m_top));
	}
	else
	{
		if(m_stick & Left)
		{
			m_area.setPosition(point(m_stickRect.m_left, m_area.m_top));
		}
		else if(m_stick & Right)
		{
			m_area.setPosition(point(parent_size.width - sz.width - m_stickRect.m_right, m_area.m_top));
		}
	}

	if(m_stick & VCenter)
	{
		m_area.setPosition(point(m_area.m_left, (parent_size.height - sz.height) / 2));
	}
	else
	{
		if(m_stick & Top)
		{
			m_area.setPosition(point(m_area.m_left, m_stickRect.m_top));
		}
		else if(m_stick & Bottom)
		{
			m_area.setPosition(point(m_area.m_left, parent_size.height - sz.height - m_stickRect.m_bottom));
		}
	}
}

bool base_window::onMoved(void)
{
	if(m_parent)
	{
		Stick();
		const Rect& pr = m_parent->getArea();
		Rect area(m_area);
		area.offset(pr.getPosition());

		m_alignmentRect.m_left = area.m_left - pr.m_left;
		m_alignmentRect.m_top = area.m_top - pr.m_top;
		m_alignmentRect.m_right = pr.m_right - area.m_right;
		m_alignmentRect.m_bottom = pr.m_bottom - area.m_bottom;
	}
	invalidate();
	EventArgs a;
	a.name = "On_Moved";
	callHandler(&a);
	return true;
}

bool base_window::onTick(float delta)
{
	TickEventArgs a;
	a.name = "On_Tick";
	a.delta = delta;
	callHandler(&a);
	return a.handled;
}

bool base_window::onTooltipShow()
{
	EventArgs a;
	a.name = "On_TooltipShow";
	callHandler(&a);
	return a.handled;
}

bool base_window::onTooltipHide()
{
	EventArgs a;
	a.name = "On_TooltipHide";
	callHandler(&a);
	return a.handled;
}

void base_window::startTick(void)
{
	m_unsubscribePending = false;
	m_system.subscribeTick(this);
}

void base_window::stopTick(void)
{
	m_unsubscribePending = true;
	m_system.setTickClear();
}

void base_window::init(xml::node& node)
{	
	xml::node setting = node("Visible");
	if(!setting.empty())
	{
		m_visible = StringToBool(setting.first_child().value());
	}
	setting = node("Align");
	if(!setting.empty())
	{
		m_alignment = StringToAlignment(setting.first_child().value());
	}
	setting = node("Stick");
	if(!setting.empty())
	{
		m_stick = StringToAlignment(setting.first_child().value());
	}
	setting = node("StickRect");
	if(!setting.empty())
	{
		m_stickRect = StringToRect(setting.first_child().value());
	}
	setting = node("Area");
	if(!setting.empty())
	{
		m_area = StringToArea(setting.first_child().value());
	}
	setting = node("Backcolor");
	if(!setting.empty())
	{
		m_backColor = StringToColor(setting.first_child().value());
	}
	setting = node("Forecolor");
	if(!setting.empty())
	{
		m_foreColor = StringToColor(setting.first_child().value());
	}
	setting = node("TabStop");
	if(!setting.empty())
	{
		m_tabstop = StringToBool(setting.first_child().value());
	}
	setting = node("Draggable");
	if(!setting.empty())
	{
		m_draggable = StringToBool(setting.first_child().value());
	}
	setting = node("AcceptDrop");
	if(!setting.empty())
	{
		m_acceptDrop = StringToBool(setting.first_child().value());
	}
	setting = node("AlwaysOnTop");
	if(!setting.empty())
	{
		m_alwaysOnTop = StringToBool(setting.first_child().value());
	}
	setting = node("Tooltip");
	if(!setting.empty())
	{
		m_tooltip = StringToBool(setting.first_child().value());
	}
	setting = node("Size");
	if(!setting.empty())
	{
		setSize(StringToSize(setting.first_child().value()));
	}
	setting = node("Pos");
	if(!setting.empty())
	{
		setPosition(StringToPoint(setting.first_child().value()));
	}
	setting = node("IgnoreInputEvents");
	if(!setting.empty())
	{
		setIgnoreInputEvents(StringToBool(setting.first_child().value()));
	}
	onMoved();
}

void base_window::parseEventHandlers(xml::node& node)
{
	if (node.empty()) return;

	xml::node handler = node.first_child();
	while(!handler.empty())
	{
		addScriptEventHandler(handler.name(), handler.first_child().value());
		handler = handler.next_sibling();
	}
}

void base_window::addScriptEventHandler(std::string name, std::string handler)
{
	if (name.empty() || handler.empty()) return;
	m_handlers[name] = handler;
}

void base_window::CallAfterRenderCallback(const Rect& dest, const Rect& clip)
{
	if (!m_afterRenderCallback) return;
	Renderer& r = m_system.getRenderer();
	r.addCallback(m_afterRenderCallback, this, dest, clip);
}

void base_window::draw(const point& offset, const Rect& clip)
{
	if(m_visible)
	{
		if(m_area.getWidth() < 1.f)
			return;

		Rect destrect(m_area);
		destrect.offset(offset);
		Rect cliprect(destrect);
		cliprect = cliprect.getIntersection(clip);

		//if (m_invalidated)
		{
			m_system.getRenderer().startCaptureForCache(this);			
			if(m_customDraw && !m_drawhandler.empty())
			{
				EventArgs a;
				a.name = "On_Draw";

				luabind::object globals = luabind::globals(m_system.getScriptSystem().getLuaState());

				globals["eventArgs"] = &a;
				ExecuteScript(a.name, m_drawhandler);
				globals["eventArgs"] = 0;
			}

			render(destrect, cliprect); // render self first
			m_system.getRenderer().endCaptureForCache(this);		
			m_invalidated = false;
		}
		//else 
		//{
		//	//if (!m_system.getRenderer().isExistInCache(this))
		//	//{
		//	//	m_system.getRenderer().startCaptureForCache(this);			
		//	//	if(m_customDraw && !m_drawhandler.empty())
		//	//	{
		//	//		EventArgs a;
		//	//		a.name = "On_Draw";
		//	//		luabind::globals (m_system.getScriptSystem().LuaState())["eventArgs"] = &a;
		//	//		ExecuteScript(a.name, m_drawhandler);
		//	//		luabind::globals (m_system.getScriptSystem().LuaState())["eventArgs"] = 0;
		//	//	}

		//	//	render(destrect, cliprect); // render self first
		//	//	m_system.getRenderer().endCaptureForCache(this);
		//	//}
		//	//else 
		//		m_system.getRenderer().drawFromCache(this);
		//}

		child_iter i = m_children.begin();
		child_iter end = m_children.end();
		while(i != end)
		{
			(*i)->draw(destrect.getPosition(), cliprect);
			++i;
		}

		// теперь скажем, что тут коллбак при отрисовке нужно сделать
		CallAfterRenderCallback(destrect,cliprect);
	}	
}

point base_window::transformToWndCoord(const point& global) const
{
	base_window* parent = m_parent;
	point out(global);
	if(parent)
	{
		out -= parent->getArea().getPosition();
		out = parent->transformToWndCoord(out);
	}
	return out;
}

point base_window::transformToRootCoord(const point& local)
{
	base_window* parent = m_parent;
	point out(local);
	if(parent)
	{
		out += parent->getArea().getPosition();
		out = parent->transformToRootCoord(out);
	}
	return out;
}

base_window* base_window::nextSibling()
{
	if (!m_parent) return this;
	
	children_list& list = m_parent->getChildren();
	if(list.size() <= 1)
		return this;
	child_iter it = std::find_if(list.begin(), list.end(), seeker(this));
	if(it == list.end())
	{
		assert(false && "Link to parent is invalid!");
	}
	return (++it != list.end()) ? (*it).get() : (*list.begin()).get();
}

base_window* base_window::prevSibling()
{
	if (!m_parent) return this;
	
	children_list& list = m_parent->getChildren();
	if(list.size() <= 1)
		return this;
	child_iter it = std::find_if(list.begin(), list.end(), seeker(this));
	if(it == list.end())
	{
		assert(false && "Link to parent is invalid!");
	}
	if(it == list.begin())
		return (*list.rbegin()).get();
	--it;
	return (*it).get();
}

//void base_window::thisset()
//{
//	if(m_system.getScriptSystem().LuaState())
//		luabind::globals(m_system.getScriptSystem().LuaState())["this"] = this;
//}

void base_window::subscribeNamedEvent(std::string name, base_window* sender, luabind::object script_callback)
{
	//if (!script_callback) return;
	//luabind::call_function<void>(std::ref(script_callback));
	//	return; //nothing to do!
//http://stackoverflow.com/questions/11529883/callback-to-lua-member-function/11529955#11529955
	if (name.empty()) return;

	NamedEventEntry entry = std::make_pair(name, sender);		
	m_scriptevents.insert(std::make_pair(entry, script_callback));

	// support for a script events
	subscribe<events::NamedEvent, base_window> (&base_window::onNamedEvent, sender);
}

void base_window::unsubscribeNamedEvent(std::string name, base_window* sender)
{
	if (name.empty()) return;

	NamedEventEntry entry = std::make_pair(name, sender);
	NamedEventsMap::iterator it = m_scriptevents.find(entry);
	if (it == m_scriptevents.end()) return;

	m_scriptevents.erase(it);
	if(sender)
		unsubscribe<events::NamedEvent>(sender);

	if(m_scriptevents.empty())
		unsubscribe<events::NamedEvent>();
}

void base_window::sendNamedEvent(std::string name)
{
	if (name.empty()) return;
	send_event(events::NamedEvent(name, this));
}

void base_window::onNamedEvent(events::NamedEvent& e)
{
	NamedEventEntry entry = std::make_pair(e.m_name, e.m_sender);
	NamedEventsMap::iterator it = m_scriptevents.find(entry);
	if (it == m_scriptevents.end()) {
		entry = std::make_pair(e.m_name, nullptr);
		it = m_scriptevents.find(entry);
		if (it == m_scriptevents.end()) return;
	}
	
	luabind::object script_callback = it->second;
	if (!script_callback) return;

	EventArgs arg("On_ScriptEvent");

	luabind::object globals = luabind::globals(m_system.getScriptSystem().getLuaState());
	globals["eventArgs"] = &arg;
	luabind::call_function<void>(std::ref(script_callback), std::string(arg.name));
	//ExecuteScript(arg.name, script);
	globals["eventArgs"] = 0;
}

std::string base_window::getEventScript(const std::string& ev)
{
	HandlerMap::iterator it = m_handlers.find(ev);
	if (it == m_handlers.end()) return std::string();

	const std::string& handler = it->second;
	return handler;	
}

bool base_window::onGameEvent(const std::string& ev)
{
	HandlerMap::iterator it = m_handlers.find(ev);
	if (it == m_handlers.end()) return false;

	std::string& handler = it->second;
	if (handler.empty()) return false;

	EventArgs arg;
	arg.name = ev.c_str();

	luabind::object globals = luabind::globals(m_system.getScriptSystem().getLuaState());

	globals["gameEventArgs"] = &arg;
	ExecuteScript(ev, handler);
	globals["gameEventArgs"] = 0;
	return arg.handled;
}

bool base_window::isChildrenOf(const base_window* wnd)
{
	if (!wnd || !m_parent) return false;
	if (m_parent == wnd) return true;
	return m_parent->isChildrenOf(wnd);
}

}
