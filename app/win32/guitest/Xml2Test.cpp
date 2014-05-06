#include "Xml2Test.h"

using namespace gui;

Xml2Test::Xml2Test(int w, int h, const char* title)
: ui_test_application(w,h,title)
{
	//m_filename = "guitest\\hud.xml";
	//m_filename = "guitest\\sampleframetest_orig.xml";
	//m_filename = "guitest\\sampleframe.xml";
	m_filename = "guitest\\test.xml";	
	//m_filename = "guitest\\testfr2.xml";	
}

Xml2Test::~Xml2Test(void)
{
}

void Xml2Test::createGUISystem()
{
	ui_test_application::createGUISystem();
	load(m_filename);
}

void Xml2Test::resetGUISystem()
{
	ui_test_application::resetGUISystem();
	
	load(m_filename);
}
