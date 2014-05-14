#pragma once

#include <guilib/guilib.h>
#include "base_application.h"
//#include <boost/timer.hpp>
#include <efsw/efsw.hpp>
#include <iostream>


class ui_test_application : public BaseApplication, public efsw::FileWatchListener
{
public:
	ui_test_application(int w, int h, const char* title);
	~ui_test_application();

	void run();

	virtual void createGUISystem();
	virtual void resetGUISystem();

	virtual bool isFinished();

	bool isInitialized() { return m_system != 0; }

	bool handleMouseMove(int x, int y);
	bool handleMouseWheel(int diff);
	bool handleMouseButton(gui::EventArgs::MouseButtons btn, gui::EventArgs::ButtonState state);
	//virtual bool handleKeyboard(UINT_PTR key, gui::EventArgs::ButtonState state);
	//bool handleChar(UINT_PTR ch);

	void handleViewportChange();

	void load(const std::string& xml);

	// from BaseApplication
	void onMousebutton(int button, int action);
	void onMousepos(int x, int y);
	void onMousewheel(int delta);
	void onKey(int key, int action);
	void onChar(int character, int action);

protected:
	void reinit();
	void resize_scene(unsigned int width, unsigned int height);	
	void update();
	void render();

	std::string getActionName(efsw::Action action)
	{
		switch (action)
		{
		case efsw::Actions::Add:		return "Add";
		case efsw::Actions::Modified:	return "Modified";
		case efsw::Actions::Delete:		return "Delete";
		case efsw::Actions::Moved:		return "Moved";
		default:						return "Bad Action";
		}
	}

	void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename = "");

protected:
	std::string m_filename;

	gui::RenderDevicePtr m_render_device;
	std::shared_ptr<gui::Renderer> m_render;
	std::shared_ptr<gui::System> m_system;		

	unsigned int m_framecount;

	bool m_active;
	double m_elapsed;

	int mouse_x, mouse_y;
	efsw::FileWatcher m_fileWatcher;
	efsw::WatchID m_watchID;
	bool m_needReload;
	//gui::FontPtr m_font;
};
