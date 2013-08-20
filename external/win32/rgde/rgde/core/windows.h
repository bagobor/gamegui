#pragma once

#include <rgde/core/windows_forward.h>
#include <rgde/core/types.h>

namespace rgde
{
	namespace core
	{
		namespace windows
		{
			class window
			{
			public:		
				explicit window(handle external_handle);
				explicit window(const std::wstring& title = L"Window");
				window(const point& pos, const size& s, const std::wstring& title = L"Window");
				window(const point& pos, const size& s, const std::wstring& title, handle parent_handle, 
					ulong style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
				virtual ~window();

				virtual handle get_handle() const { return m_hwnd; }
				bool is_created() const {return 0 != m_hwnd;}				
				
				bool set_state(int state);
				bool show() {return set_state(SW_SHOW);}
				bool hide() {return set_state(SW_HIDE);}

				bool set_position(const point& p);
				point get_position() const;

				bool set_size(const size& s);
				size get_size() const;

				bool move_window(const point& p, const size& s);

				bool update();

				const std::wstring& get_title() const {return m_window_title;}
				const std::wstring& get_class_name() const {return m_class_name;}

			protected:
				bool create(const point& pos, const size& s, ulong style);
				bool register_class();

				virtual result wnd_proc(ushort message, uint wparam, long lparam );

			protected:
				handle		m_hwnd;
				handle		m_parent_hwnd;
				HINSTANCE	m_instance;
				WNDCLASS	m_wc;

				std::wstring m_class_name;
				std::wstring m_window_title;

				bool m_using_external_handle;

			private:
				static result __stdcall dispatch( handle hWnd, ushort message, uint wParam, long lParam );
				static	std::map<handle, window*>	m_map;
			};

		}
	}
}