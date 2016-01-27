#include "ui_sample.h"

using namespace gui;

ui_sample::ui_sample(int w, int h, const char* title)
: ui_application(w,h,title)
{
	m_filename = "guitest/test.xml";	
}

ui_sample::~ui_sample(void)
{
}

void ui_sample::createGUISystem()
{
	ui_application::createGUISystem();
	load(m_filename);
}

void ui_sample::resetGUISystem()
{
	ui_application::resetGUISystem();
	load(m_filename);
}
