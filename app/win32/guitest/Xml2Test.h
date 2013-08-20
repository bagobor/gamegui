#pragma once

#include "uitest.h"


class Xml2Test : public rgde::ui_test_application
{
public:
	Xml2Test(int x, int y, int w, int h, const std::wstring& title);
	~Xml2Test(void);
		
	virtual void createGUISystem();
	virtual void resetGUISystem();

protected:
	gui::window_ptr m_wnd;
};
