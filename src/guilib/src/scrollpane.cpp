#include "StdAfx.h"

#include "scrollpane.h"

#include "slider.h"
#include "eventtypes.h"

namespace gui
{

ScrollPane::ScrollPane(System& sys, const std::string& name) 
	: base_window(sys, name)
	, m_scroll(0)
	, m_target(0)
{
}

ScrollPane::~ScrollPane(void)
{
}

void ScrollPane::setTarget(base_window* target)
{
	m_target = target;
	if(target && m_scroll)
	{
		add(window_ptr(target));
		bringToBack(target);
		layoutTarget();
		subscribe<events::SizedEvent, ScrollPane> (&ScrollPane::onTargetSized, m_target);				
	}
	else if(!target)
	{
		unsubscribe<events::SizedEvent>();
	}

	invalidate();
}

void ScrollPane::layoutTarget()
{
	if(m_target && m_scroll)
	{
		float doc = m_target->getSize().height;
		m_scroll->setDocumentSize(doc);
		float h = m_area.getHeight();

		float pos = m_scroll->getScrollPosition();
		float height = doc > h ? doc - h : doc;
		m_target->setPosition(point(0.f, -pos*height));
	}
}

bool ScrollPane::onLoad(void)
{
	window_ptr scroll = child("scrollbar");
	if(scroll)
	{
		m_scroll = dynamic_cast<ScrollBar*>(scroll.get());
		if(m_scroll)
		{
			subscribe<events::TrackEvent, ScrollPane> (&ScrollPane::onTrack, m_scroll);
			Size sz(m_area.getSize());
			sz.width = m_scroll->getSize().width;
			m_scroll->setSize(sz);
			m_scroll->setAlwaysOnTop(true);
		}
	}

	return base_window::onLoad();
}

bool ScrollPane::onSized(bool update)
{
	base_window::onSized();

	if(m_scroll)
	{
		if(m_target)
		{
			float doc = m_target->getSize().height;
			m_scroll->setDocumentSize(doc);
		}
	}

	return true;
}

bool ScrollPane::onMouseWheel(int delta)
{
	if(m_scroll)
	{
		return m_scroll->onMouseWheel(delta);
	}
	return false;
}

void ScrollPane::onTrack(const events::TrackEvent& e)
{
	if(m_target && m_scroll)
	{
		float pos = m_scroll->getScrollPosition();
		float doc = m_target->getSize().height - m_area.getHeight();
		m_target->setPosition(point(0.f, -pos*doc));		
	}
}

void ScrollPane::onTargetSized(const events::SizedEvent& e)
{
	layoutTarget();	
}

void ScrollPane::init(xml::node& node)
{
	base_window::init(node);
	
	//TODO: ???
}

}