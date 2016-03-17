#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#include	<shlobj.h>

#include "ui_application.h"
#include <guiplatform/renderer_ogl.h>

#include <iostream>
#include <functional>

#include "../common/gui_filesystem_win.h"
#include "../common/gui_log_win.h"

using namespace gui;

ApplicationGUI::ApplicationGUI(int w, int h, const char* title)
	: BaseApplicationGLFW(w, h, title)
	, m_render(NULL)
	, m_system(NULL)
	, m_elapsed(0)
	, m_active(true)
	, m_needReload(false)
{
	update();
	m_filename = "guitest/test.xml";
}

ApplicationGUI::~ApplicationGUI()
{
	m_fileWatcher.removeWatch(m_watchID);
	m_system.reset();
	m_render.reset();
	m_log.reset();
}

void ApplicationGUI::run()
{	
	createGUISystem();
	BaseApplicationGLFW::run();
}

void ApplicationGUI::createGUISystem()
{
	filesystem_ptr fs = std::make_shared<platform_win32::filesystem>("/data/");
	m_log = std::make_shared<platform_win32::log>();

	m_watchID = m_fileWatcher.addWatch(fs->get_root_dir(0), this, true);

	m_render_device = std::make_shared<gui::ogl_platform::RenderDeviceGL>(fs, 1024);
	m_render = std::make_shared<gui::Renderer>(*m_render_device, fs);

	if(m_system)
		m_system.reset();
	
	m_system = std::make_shared<System>(*m_render, "default", nullptr, *m_log);

	if(m_system)
	{
		Cursor& cursor = m_system->getCursor();
		cursor.setType("CursorNormal");
	}

	m_fileWatcher.watch();
	load(m_filename);
}

void ApplicationGUI::resetGUISystem()
{
	if(m_render)
		m_render->clearRenderList();

	if(m_system)
		m_system->reset();

	load(m_filename);
}

void ApplicationGUI::update()
{
	if (m_needReload) {
		m_needReload = false;
		resetGUISystem();
		m_framecount = 0;
	}

	m_framecount++;
	if(m_system)
	{
		m_system->tick(frameDt());
		m_system->draw();
	}	
}

void ApplicationGUI::render()
{
	if (m_render) {
		m_render->setOriginalSize(gui::Size(width(), height()));
	}

	if (m_system)
	{
		m_system->render();
	}
}

bool ApplicationGUI::isFinished()
{
	return false;
}

void ApplicationGUI::onWindowSize(int w, int h) {
	BaseApplicationGLFW::onWindowSize(w, h);

	gui::RenderDevice::ViewPort vp = {
		0,0,w,h
	};

	if (m_render_device) {
		m_render_device->setViewport(vp);
	}

	handleViewportChange();
}

void ApplicationGUI::handleViewportChange()
{
	if(!m_system) return;

	gui::event e = {0};
	e.type = gui::event_viewport_resize;
	m_system->handle_event(e);
}


bool ApplicationGUI::handleMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
{
	if (!m_system) {
		return false;
	}

	gui::event e = {0};
	e.type = gui::event_mouse | gui::mouse_button;
	e.type |= (state == EventArgs::Down) ? gui::event_key_down : gui::event_key_up;

	switch(btn) {
		case EventArgs::Left:
			e.mouse.button = gui::button_left;
			break;
		case EventArgs::Middle:
			e.mouse.button = gui::button_middle;
			break;
		case EventArgs::Right:
			e.mouse.button = gui::button_right;
			break;
	}

	return m_system->handle_event(e);
}

void ApplicationGUI::onMousebutton(int button, int action) {
	if (!m_system || button > 2 || action > 1) return;

	int gui_buttons_map[] = { gui::button_left, gui::button_right, gui::button_middle };
	gui::event_type gui_actions_map[] = { gui::event_key_up, gui::event_key_down };

	gui::event e = { 0 };
	e.type = gui::event_mouse | gui::mouse_button;
	e.type |= gui_actions_map[action];
	e.mouse.button = gui_buttons_map[button];
	m_system->handle_event(e);
}

void ApplicationGUI::onMousepos(int x, int y) {
	mouse_x = x;
	mouse_y = y;

	if (!m_system) return;

	gui::event e = {0};
	e.type = gui::event_mouse | gui::mouse_move;
	e.mouse.x = mouse_x;
	e.mouse.y = mouse_y;
	m_system->handle_event(e);
}

void ApplicationGUI::onMousewheel(int delta) {
	if (!m_system) return;

	gui::event e = {0};
	e.type = gui::event_mouse | gui::mouse_wheel;
	e.mouse.delta = delta;
	m_system->handle_event(e);
}

void ApplicationGUI::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
{
	if (efsw::Actions::Modified == action) {
		m_needReload = true;
	}
	printf("DIR (%s) FILE (%s to %s) has event %s", dir.c_str(), oldFilename.c_str(), filename.c_str(), getActionName(action).c_str());
}

void ApplicationGUI::onKey(int key, int action) {

	if (!m_filename.empty() && key == 294 && action == 1)
	{
		resetGUISystem();
		return;
	}
}

void ApplicationGUI::onChar(int character, int action) {
	if (!m_system) return;
}
//
//bool ui_test_application::handleKeyboard(UINT_PTR key, EventArgs::ButtonState state)
//{
//	if(!m_system) return false;
//
//	if (!m_filename.empty())
//		if((EventArgs::Keys)key == EventArgs::K_F5 && state == EventArgs::Down)
//		{
//			resetGUISystem();
//			return true;
//		}
//
//	gui::event e = {0};
//	e.type = gui::event_keyboard;
//	e.type |= (state == EventArgs::Down) ? gui::event_key_down : gui::event_key_up;
//	e.keyboard.key = (gui::EventArgs::Keys)key;
//	return m_system->handle_event(e);
//}
//
//bool ui_test_application::handleChar(UINT_PTR ch)
//{
//	if(m_system){
//		gui::event e = {0};
//		e.type = gui::event_char;
//		e.text.code = ch;
//		return m_system->handle_event(e);
//		//return m_system->handleChar((unsigned int)ch);
//	}
//	else
//		return false;
//}



void ApplicationGUI::load(const std::string& xml)
{
	if(!m_system) return;
	gui::WindowBase* wnd = m_system->loadXml(xml);
}

const std::string& ApplicationGUI::getActionName(efsw::Action action)
{
	static std::string action_names[] = {
		"Bad Action",
		"Add",
		"Delete",
		"Modified"
		"Moved"
	};
	action = (efsw::Actions::Action)(action < 0 ? 0 : action);
	action = (efsw::Actions::Action)(action <= efsw::Actions::Moved ? action : 0);
	return action_names[action];
}
