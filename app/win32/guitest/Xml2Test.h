#pragma once

#include "uitest.h"


class Xml2Test : public ui_test_application
{
public:
	Xml2Test(int w, int h, const char* title);
	~Xml2Test(void);
		
	virtual void createGUISystem();
	virtual void resetGUISystem();

protected:
	gui::window_ptr m_wnd;
};
