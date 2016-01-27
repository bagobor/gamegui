#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#include	<shlobj.h>

#include "ui_application.h"
#include <guiplatform/renderer_ogl.h>

#include <iostream>
#include <functional>

using namespace gui;

class gui_log : public log
{
public: 
	gui_log() : 
	  m_hFile(INVALID_HANDLE_VALUE)
	{
		  wchar_t	wpath[MAX_PATH];
		  // init app data path
		  SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, wpath);

		  char buff[256];
		  int size = GetModuleFileNameA(NULL, buff, 256);

		  std::string path(buff, size);

		  int pos = path.find_last_of('\\');

		  path = path.substr(0, pos);


		  std::string log = path + "/guitest.log";
		  //std::wstring log(wpath);
		  //log += L"\\RGDEngine\\guitest.log";
		  m_hFile = CreateFileA(log.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	}

	~gui_log() {
		if(m_hFile != INVALID_HANDLE_VALUE)
			CloseHandle(m_hFile);
	}

	void write(log::level l, const std::string& msg) 
	{
		static const char* type2str[log::log_levels_num] = {"sys", "msg", "wrn", "ERR", "CRITICAL"};
		const char* type = type2str[l];

		SYSTEMTIME st;
		GetLocalTime(&st);

		static char con_timestamp[32] = {0};
		_snprintf(con_timestamp, 32, "[%02d:%02d:%02d][%s] ", st.wHour, st.wMinute, st.wSecond, type);
		std::cout << con_timestamp << msg.c_str() << std::endl;

		if(m_hFile == INVALID_HANDLE_VALUE)
			return;

		static char timestamp[32] = {0};
		_snprintf(timestamp, 32, "[%04d.%02d.%02d %02d:%02d:%02d][%s] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, type);
	
		std::string m = timestamp + msg + "\n";
		DWORD len = (DWORD)m.length();
		WriteFile(m_hFile, m.c_str(), len, &len, 0);
	}

	HANDLE m_hFile;
};

gui_log g_log;

#include "../common/filesystem_win.h"

ui_application::ui_application(int w, int h, const char* title)
	: BaseApplication(w, h, title)
	, m_render(NULL)
	, m_system(NULL)
	//, window(math::vec2i(x, y), math::vec2i(w, h), title, 0, WS_BORDER | WS_CAPTION | WS_SYSMENU)
	//, m_render_device(m_filesystem)
	, m_elapsed(0)
	, m_active(true)
	, m_needReload(false)
{
	using namespace std::placeholders;
	env.render_cb = std::bind(&ui_application::render, this);
	env.update_cb = std::bind(&ui_application::update, this);

	//show();
	update();	
}

ui_application::~ui_application()
{
	m_fileWatcher.removeWatch(m_watchID);
	m_system.reset();
	m_render.reset();
}

void ui_application::run()
{	
	createGUISystem();
	BaseApplication::run();
}

void ui_application::createGUISystem()
{
	filesystem_ptr fs = std::make_shared<win::filesystem>("/data/");

	m_watchID = m_fileWatcher.addWatch(fs->get_root_dir(0), this, true);

	m_render_device = std::make_shared<gui::ogl_platform::RenderDeviceGL>(fs, 1024);
	m_render = std::make_shared<gui::Renderer>(*m_render_device, fs);

	if(m_system)
		m_system.reset();

	m_system = std::make_shared<System>(*m_render, "default", nullptr, g_log);

	if(m_system)
	{
		//::ShowCursor(FALSE);
		Cursor& cursor = m_system->getCursor();
		cursor.setType("CursorNormal");
		//m_font = m_system->getWindowManager().loadFont("exotibi");
	}

	m_fileWatcher.watch();
}

void ui_application::resetGUISystem()
{
	if(m_render)
		m_render->clearRenderList();

	if(m_system)
		m_system->reset();	
}

void ui_application::update()
{
	if (m_needReload) {
		m_needReload = false;
		resetGUISystem();
		m_framecount = 0;
	}

	m_framecount++;
	if(m_system)
	{
		m_system->tick(env.dt);
		m_system->draw();
	}	
}

void ui_application::render()
{
	if (m_render) {
		m_render->setOriginalSize(gui::Size(width(), height()));
	}

	if (m_system)
	{
		m_system->render();
	}
}

bool ui_application::isFinished()
{
	return false;
}

void ui_application::onWindowSize(int w, int h) {
	BaseApplication::onWindowSize(w, h);

	gui::RenderDevice::ViewPort vp = {
		0,0,w,h
	};

	if (m_render_device) {
		m_render_device->setViewport(vp);
	}

	handleViewportChange();
}

void ui_application::handleViewportChange()
{
	if(!m_system) return;

	gui::event e = {0};
	e.type = gui::event_viewport_resize;
	m_system->handle_event(e);
}


bool ui_application::handleMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
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

void ui_application::onMousebutton(int button, int action) {
	if (!m_system || button > 2 || action > 1) return;

	int gui_buttons_map[] = { gui::button_left, gui::button_right, gui::button_middle };
	gui::event_type gui_actions_map[] = { gui::event_key_up, gui::event_key_down };

	gui::event e = { 0 };
	e.type = gui::event_mouse | gui::mouse_button;
	e.type |= gui_actions_map[action];
	e.mouse.button = gui_buttons_map[button];
	m_system->handle_event(e);
}

void ui_application::onMousepos(int x, int y) {
	mouse_x = x;
	mouse_y = y;

	if (!m_system) return;

	gui::event e = {0};
	e.type = gui::event_mouse | gui::mouse_move;
	e.mouse.x = mouse_x;
	e.mouse.y = mouse_y;
	m_system->handle_event(e);
}

void ui_application::onMousewheel(int delta) {
	if (!m_system) return;

	gui::event e = {0};
	e.type = gui::event_mouse | gui::mouse_wheel;
	e.mouse.delta = delta;
	m_system->handle_event(e);
}

void ui_application::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
{
	if (efsw::Actions::Modified == action) {
		m_needReload = true;
	}
	printf("DIR (%s) FILE (%s to %s) has event %s", dir.c_str(), oldFilename.c_str(), filename.c_str(), getActionName(action).c_str());
}

void ui_application::onKey(int key, int action) {

	if (!m_filename.empty() && key == 294 && action == 1)
	{
		resetGUISystem();
		return;
	}
}

void ui_application::onChar(int character, int action) {
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



void ui_application::load(const std::string& xml)
{
	if(!m_system) return;
	gui::base_window* wnd = m_system->loadXml(xml);
}

const std::string& ui_application::getActionName(efsw::Action action)
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
