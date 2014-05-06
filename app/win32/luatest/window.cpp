#include "stdafx.h"
#include "window.h"

#include <guilib.h>
#include <renderer_ogl.h>
extern "C"
{
#include <lstate.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}


void LogEvent(HANDLE hFile, gui::log::level level, const std::string& message)
{
	if(hFile == INVALID_HANDLE_VALUE)
		return;

	std::string type;
	switch(level)
	{
	case gui::log::system:
		type = "sys";
		break;
	case gui::log::warning:
		type = "wrn";
		break;
	case gui::log::error:
		type = "ERROR";
		break;
	case gui::log::critical:
		type = "CRITICAL";
		break;
	default:
		type = "msg";
		break;
	};

	SYSTEMTIME st;
	GetLocalTime(&st);

	char timestamp[32] = {0};
	_snprintf(timestamp, 32, "[%04d.%02d.%02d %02d:%02d:%02d][%s] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, type.c_str());

	char con_timestamp[32] = {0};
	_snprintf(con_timestamp, 32, "[%02d:%02d:%02d][%s] ", st.wHour, st.wMinute, st.wSecond, type.c_str());

	std::string m(timestamp);
	m += message;
	std::cout << (std::string(con_timestamp) + message).c_str() << std::endl;

	if(hFile != INVALID_HANDLE_VALUE)
	{
		m += "\n";
		DWORD len = (DWORD)m.length();
		WriteFile(hFile, m.c_str(), len, &len, 0);
	}
}


//void LogEvent(HANDLE hFile, gui::LogLevel level, const std::string& message);

TestWindow::TestWindow(int x, int y, int w, int h, const std::wstring& title)
	: m_render(NULL)
	, m_system(NULL)
	, m_hFile(INVALID_HANDLE_VALUE)
	, window(rgde::math::vec2i(x, y), rgde::math::vec2i(w, h), title, 0, WS_BORDER | WS_CAPTION | WS_SYSMENU)
	, m_render_device(get_handle(), m_filesystem)
	, m_elapsed(0)
	, m_active(true)
{
	m_state = lua_open();
	if (m_state)
		luaL_openlibs(m_state);

	wchar_t	wpath[MAX_PATH];
	// init app data path
	SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, wpath);

	std::wstring log(wpath);
	log += L"\\RGDEngine\\guitest.log";
	m_hFile = CreateFileW(log.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	wchar_t buf[512];
	GetModuleFileNameW(NULL, &buf[0], 512);

	SetCurrentDirectoryW(L"../");

	show();
	update(0.0f);	
}

TestWindow::~TestWindow()
{
	if(m_system)
		delete m_system;

	if(m_render)
		delete m_render;

	if(m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
	
	lua_close(m_state);
}

void TestWindow::run()
{	
	createGUISystem();

	m_timer.restart();

	while( is_created() )
	{
		if( !do_events() && m_active)
		{
			m_elapsed = m_timer.elapsed();
			update((float)m_elapsed);
			render();
		}
	}
}

#include "../common/gui_filesystem.h"

void TestWindow::createGUISystem()
{
	if (m_render) delete m_render;
	if(m_system) delete m_system;

	filesystem_ptr fs(new gui_filesystem(m_filesystem, "/"));

	m_render = gui::ogl_platform::CreateRenderer(m_render_device, fs, 1024);
	assert(m_render && "Can't create GUI renderer!");
	m_system = new gui::System(*m_render, "default", m_state);
	assert(m_system && "Can't create GUI System!");

	//::ShowCursor(FALSE);
	gui::Cursor& cursor = m_system->getCursor();
	cursor.setType("CursorNormal");

	m_font = m_system->getWindowManager().loadFont("default");
	load("guitest\\test.xml");
}

void TestWindow::resetGUISystem()
{
	if(m_render)
		m_render->clearRenderList();

	if(m_system)
		m_system->reset();
	load("guitest\\test.xml");
}

void TestWindow::update(float delta)
{
	m_framecount++;
	if(m_system)
	{
		m_system->tick(delta);
		m_system->draw();
	}
	
}

bool TestWindow::do_events()
{
	MSG msg = {0};
	if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
		return true;
	}
	return false;
}

void TestWindow::render()
{
	m_render_device.frame_begin();
	m_render_device.clear(rgde::math::color::Black);

	if (m_system)
		m_system->render();

	m_render_device.frame_end();
	m_render_device.present();
}

bool TestWindow::isFinished() 
{
	return false;
}

rgde::core::windows::result TestWindow::wnd_proc(rgde::ushort message, rgde::uint wparam, long lparam )
{
	gui::event e = {0};

	switch (message)
	{
	case WM_CHAR:
		if(m_system) {
			e.type = gui::event_char;
			e.text.code = wparam;
			return m_system->handle_event(e);
		}
		return 0;

	case WM_LBUTTONDOWN:
		if(m_system) {
			e.type = gui::event_mouse | gui::mouse_button | gui::event_key_down;
			e.mouse.button = gui::button_left;
			m_system->handle_event(e);
			//m_system->handleMouseButton(gui::EventArgs::Left, gui::EventArgs::Down);
		}
		return 0;

	case WM_LBUTTONUP:
		if(m_system){
			e.type = gui::event_mouse | gui::mouse_button | gui::event_key_up;
			e.mouse.button = gui::button_left;
			m_system->handle_event(e);
			//m_system->handleMouseButton(gui::EventArgs::Left, gui::EventArgs::Up);
		}
		return 0;

	case WM_RBUTTONDOWN:
		if(m_system){
			e.type = gui::event_mouse | gui::mouse_button | gui::event_key_down;
			e.mouse.button = gui::button_right;
			m_system->handle_event(e);
			//m_system->handleMouseButton(gui::EventArgs::Right, gui::EventArgs::Down);
		}
		return 0;

	case WM_RBUTTONUP:
		if(m_system){
			e.type = gui::event_mouse | gui::mouse_button | gui::event_key_up;
			e.mouse.button = gui::button_right;
			m_system->handle_event(e);
			//m_system->handleMouseButton(gui::EventArgs::Right, gui::EventArgs::Up);
		}
		return 0;

	case WM_MBUTTONDOWN:
		if(m_system) {
			e.type = gui::event_mouse | gui::mouse_button | gui::event_key_down;
			e.mouse.button = gui::button_middle;
			m_system->handle_event(e);
			//m_system->handleMouseButton(gui::EventArgs::Middle, gui::EventArgs::Down);
		}
		return 0;

	case WM_MBUTTONUP:
		if(m_system) {
			e.type = gui::event_mouse | gui::mouse_button | gui::event_key_up;
			e.mouse.button = gui::button_middle;
			m_system->handle_event(e);
			//m_system->handleMouseButton(gui::EventArgs::Middle, gui::EventArgs::Up);
		}
		return 0;

	case WM_ACTIVATE:	// Watch For Window Activate Message
		m_active = !HIWORD(wparam);// Check Minimization State
		return 0;

	case WM_KEYDOWN:
		{
			if ('Q' == wparam || 'q' == wparam || VK_ESCAPE == wparam)
				exit(0);

			if(m_system) {
				e.type = gui::event_keyboard | gui::event_key_down;
				e.keyboard.key = (gui::EventArgs::Keys)wparam;
				m_system->handle_event(e);
				//m_system->handleKeyboard((gui::EventArgs::Keys)wparam, gui::EventArgs::Down);
			}

			return 0;
		}

	case WM_KEYUP:
		{
			if(VK_F5 == wparam)
			{
				resetGUISystem();
				return true;
			}
			if(m_system) {
				e.type = gui::event_keyboard | gui::event_key_up;
				e.keyboard.key = (gui::EventArgs::Keys)wparam;
				m_system->handle_event(e);
				//m_system->handleKeyboard((gui::EventArgs::Keys)wparam, gui::EventArgs::Up);
			}
		}
		return 0;

	case WM_SIZE:
		//resize_scene(LOWORD(lparam), HIWORD(lparam));
		return 0;

	case WM_MOUSEWHEEL:
		{
			int delta = GET_WHEEL_DELTA_WPARAM(wparam);
			if(m_system) {
				e.type = gui::event_mouse | gui::mouse_wheel;
				e.mouse.delta = delta;//(gui::EventArgs::Keys)wparam;
				m_system->handle_event(e);
				//m_system->handleMouseWheel(delta);
			}
		}
		return 0;

	case WM_MOUSEMOVE:
		if(m_system) {
			e.type = gui::event_mouse | gui::mouse_move;
			e.mouse.x = LOWORD(lparam);
			e.mouse.x = HIWORD(lparam);
			return m_system->handle_event(e);
		}
		return 0;
	}
	return window::wnd_proc(message, wparam, lparam);
}

void TestWindow::handleViewportChange()
{
	if(m_system) {
		gui::event e = {0};
		e.type = gui::event_viewport_resize;
		m_system->handle_event(e);
		//return m_system->handleViewportChange();
	}
}




void TestWindow::load(const std::string& xml)
{
	if(m_system)
	{
		if (gui::base_window* wnd = m_system->loadXml(xml))
		{

		}
	}
}

void TestWindow::OnLostDevice(void)
{
	try
	{
		m_render->OnLostDevice();
	}
	catch(...)
	{	
	}		
}

HRESULT TestWindow::OnResetDevice(void)
{
	try
	{
		m_render->OnResetDevice();
		handleViewportChange();
	}
	catch(...)
	{
		return S_FALSE;
	}
	return S_OK;
}