#pragma once

#include <guilib/guilib.h>
#include "base_application.h"
//#include <boost/timer.hpp>


class ui_test_application : public BaseApplication
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


protected:
	void reinit();
	void resize_scene(unsigned int width, unsigned int height);	
	void update();
	void render();

protected:
	std::string m_filename;

	gui::RenderDevicePtr m_render_device;
	std::shared_ptr<gui::Renderer> m_render;
	std::shared_ptr<gui::System> m_system;		

	unsigned int m_framecount;

	bool m_active;
	double m_elapsed;
	//gui::FontPtr m_font;
};
