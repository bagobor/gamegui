#pragma once

#include <guilib.h>

//#include <boost/timer.hpp>

namespace rgde
{
	class ui_test_application
	{
	public:
		ui_test_application(int x, int y, int w, int h, const std::wstring& title);
		~ui_test_application();

		void run();

		virtual void createGUISystem();
		virtual void resetGUISystem();

		virtual bool isFinished();

		bool isInitialized() { return m_system != 0; }

		bool handleMouseMove(int x, int y);
		bool handleMouseWheel(int diff);
		bool handleMouseButton(gui::EventArgs::MouseButtons btn, gui::EventArgs::ButtonState state);
		virtual bool handleKeyboard(UINT_PTR key, gui::EventArgs::ButtonState state);
		bool handleChar(UINT_PTR ch);

		void handleViewportChange();

		void load(const std::string& xml);

		virtual bool OnResetDevice();
		virtual void	OnLostDevice();

	protected:
		void reinit();

		void resize_scene(unsigned int width, unsigned int height);
		bool do_events();

		void update(float delta);
		void render();

		//virtual core::windows::result wnd_proc(ushort message, uint wparam, long lparam );

	protected:
		std::string m_filename;
		/*
		render::device m_render_device;
		core::vfs::system m_filesystem;*/

		gui::Renderer* m_render;
		gui::System* m_system;

		unsigned int m_framecount;

		bool m_active;
		double m_elapsed;
		std::timer m_timer;
		//gui::FontPtr m_font;
	};

}

