#pragma once

#include <guilib/guilib.h>
#include "base_application.h"
#include <efsw/efsw.hpp>
#include <iostream>

namespace gui
{
	namespace platform_win32
	{
		class log;
	}
}

class ApplicationGUI : public BaseApplicationGLFW, public efsw::FileWatchListener
{
public:
	ApplicationGUI(int w, int h, const char* title);
	~ApplicationGUI();

	void run();

	virtual void createGUISystem();
	virtual void resetGUISystem();

	virtual bool isFinished();

	bool isInitialized() { return m_system != 0; }

	bool handleMouseMove(int x, int y);
	bool handleMouseWheel(int diff);
	bool handleMouseButton(gui::EventArgs::MouseButtons btn, gui::EventArgs::ButtonState state);

	void handleViewportChange();

	void load(const std::string& xml);

	// from BaseApplicationGLFW
	void onMousebutton(int button, int action);
	void onMousepos(int x, int y);
	void onMousewheel(int delta);
	void onKey(int key, int action);
	void onChar(int character, int action);

protected:
	virtual void onWindowSize(int w, int h);

	//void reinit();
	//void resize_scene(unsigned int width, unsigned int height);	
	void update();
	void render();

	const std::string& getActionName(efsw::Action action);
	void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename = "");

protected:
	std::string m_filename;

	gui::RenderDevicePtr m_render_device;
	std::shared_ptr<gui::Renderer> m_render;
	std::shared_ptr<gui::System> m_system;
	std::shared_ptr<gui::log> m_log;

	unsigned int m_framecount;

	bool m_active;
	double m_elapsed;

	int mouse_x, mouse_y;
	efsw::FileWatcher m_fileWatcher;
	efsw::WatchID m_watchID;
	bool m_needReload;
};
