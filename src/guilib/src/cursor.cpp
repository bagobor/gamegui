#include "StdAfx.h"
#include "renderer.h"
#include "cursor.h"
#include "imagesetmanager.h"

namespace gui
{

Cursor::Cursor(Renderer& r) 
	: m_render(r)
	, m_currentCursor(0)
	, m_visible(true)
{
}

Cursor::~Cursor()
{
}

void Cursor::setPosition(point pt) 
{
	m_cursorPos = pt;
}

point Cursor::getPosition() const
{	
	Size vp = m_render.getViewportSize();
	if(m_render.isAutoScale() && vp.width && vp.height)
	{
		Size sz = m_render.getOriginalSize();
		float cx = sz.width / vp.width;
		float cy = sz.height / vp.height;
		
		return point(m_cursorPos.x * cx, m_cursorPos.y * cy);
	}
	else
	{
		return m_cursorPos;
	}
}


void Cursor::setType(std::string curname)
{
	if(m_cursors)
	{
		m_currentCursor = (*m_cursors)[curname];
	}
}

std::string Cursor::getType() const
{
	if(m_currentCursor)
	{
		return m_currentCursor->name();
	}
	return "";
}

void Cursor::render()
{
	if(m_currentCursor)
	{
		Rect dest(getPosition(), m_currentCursor->size());
		m_render.immediateDraw(*m_currentCursor, dest, 0.f, Rect(point(0.f,0.f), m_render.getViewportSize()), Color(1.f, 1.f, 1.f));
	}
}

}