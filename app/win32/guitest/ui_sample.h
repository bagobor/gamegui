#pragma once

#include "ui_application.h"


class ui_sample : public ui_application
{
public:
	ui_sample(int w, int h, const char* title);
	~ui_sample(void);
		
	virtual void createGUISystem();
	virtual void resetGUISystem();

protected:
	gui::window_ptr m_wnd;
};
