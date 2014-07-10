#include "stdafx.h"
#include "menu.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"
#include "font.h"

#include "../xml/xml.h"


namespace gui
{
Menu::Menu(System& sys, const std::string& name) :
	Panel(sys, name),
	m_fadein(0.f),
	m_fadeout(0.f),
	m_itemHeight(16.f),
	m_maxWidth(512.f),
	m_minWidth(48.f),
	m_margin(1.f),
	m_selected((size_t)-1),
	m_selImg(0)
{
	m_items.reserve(32);
	m_actualWidth = m_minWidth;
}

Menu::~Menu(void)
{
	m_items.clear();
}

namespace
{
	struct seeker
	{
		const base_window* m_ptr;
		seeker(const base_window* ptr) : m_ptr(ptr){}
		bool operator()(window_ptr obj) 
		{
			return obj ? (obj.get() == m_ptr) : false;
		}
	};
}

void Menu::rise()
{
	if (getDisableRise()) return;
	if(m_parent)
	{
		children_list& children = m_parent->getChildren();
		child_iter it = std::find_if(children.begin(), children.end(), seeker(this));
		if(it != children.end())
		{
			children.splice(children.end(), children, it);
		}
	}
}

void Menu::setFont(const std::string& font)
{
	if(font.empty())
		return;

	m_font = m_system.getWindowManager().loadFont(font);
}

void Menu::reset(void)
{
	m_system.setContextMenuState(false);
	setVisible(false);
}

void Menu::show(point& pt)
{
	m_area.setPosition(m_system.getCursor().getPosition() - pt);
	rise();
	m_system.setContextMenuState(true);
	setVisible(true);
}

void Menu::render(const Rect& finalRect, const Rect& finalClip)
{
	Panel::render(finalRect, finalClip);
	point left = finalRect.getPosition();
	
	Rect itemrect(left, Size(m_actualWidth, m_itemHeight));	
	Rect iconrect(left, Size(m_itemHeight, m_itemHeight));
	
	float h = 0.f;
	Renderer& r = m_system.getRenderer();

	MenuItemVector::iterator i = m_items.begin();
	MenuItemVector::iterator end = m_items.end();
	while(i != end)
	{
		MenuItem& item = (*i);
		bool selected = false;
		size_t index = i - m_items.begin();
		selected = (index == m_selected);

		if(selected && m_selImg)
		{
			Rect rc(0.f, h, 2 * m_margin + m_itemHeight + m_actualWidth, h + 2 * m_margin + m_itemHeight);
			rc.offset(finalRect.getPosition());
			r.draw(*m_selImg, rc, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
		}

		if(item.icon)
		{
			iconrect.setPosition(point(m_margin, h + m_margin));
			iconrect.offset(left);
			r.draw(*item.icon, iconrect, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);
		}		

		itemrect.setPosition(point(3 * m_margin + m_itemHeight, h + m_margin));
		itemrect.offset(left);
		m_font->drawText(item.text, itemrect, 1.0f, finalClip, LeftAligned, selected ? item.selcol : item.col, 1.f, 1.f);		

		h += 2 * m_margin + m_itemHeight;

		++i;
	}
}

void Menu::addItem(MenuItem item)
{
	invalidate();
	std::string& s = item.text;
	float w = m_font->getTextExtent(s);
	w = std::min(std::max(w, m_minWidth), m_maxWidth);	
	m_actualWidth = m_actualWidth > w ? m_actualWidth : w;

	m_area.setWidth(4 * m_margin + m_actualWidth + m_itemHeight);
	m_area.setHeight(m_area.getHeight() + 2 * m_margin + m_itemHeight);

	m_items.push_back(item);
}

MenuItem* Menu::getItem(std::string text)
{
	MenuItemVector::iterator i = std::find_if(m_items.begin(), m_items.end(), itemfinder(text));
	if(i != m_items.end())
		return &(*i);
	return 0;
}

MenuItem* Menu::getItem(size_t id)
{
	if(id < m_items.size())
		return &m_items[id];
	return 0;
}

void Menu::clear()
{
	invalidate();
	m_selected = (size_t)-1;
	m_items.clear();
	m_area.setHeight(0.f);
}

void Menu::init(xml::node& node)
{
	Panel::init(node);

	xml::node setting = node("Font");
	if(!setting.empty())
	{
		m_font = m_system.getWindowManager().loadFont(setting.first_child().value());
	}
	setting = node("MinWidth");
	if(!setting.empty())
	{
		m_minWidth = (float)atof(setting.first_child().value());
	}
	setting = node("MaxWidth");
	if(!setting.empty())
	{
		m_maxWidth = (float)atof(setting.first_child().value());
	}
	setting = node("Margin");
	if(!setting.empty())
	{
		m_margin = (float)atof(setting.first_child().value());
	}
	setting = node("ItemHeight");
	if(!setting.empty())
	{
		m_itemHeight = (float)atof(setting.first_child().value());
	}

	xml::node frame = node("SelectImagery");
	if(!frame.empty())
	{
		m_imgset = m_system.getWindowManager().loadImageset(frame["Imageset"].value());
		if(m_imgset)
		{
			const Imageset& set = *m_imgset;
			m_selImg = set[frame["Image"].value()];
		}
	}
}

bool Menu::onMouseEnter(void)
{
	m_selected = (size_t)-1;
	onMouseMove();
	return true;
}

bool Menu::onMouseLeave(void)
{
	invalidate();
	m_selected = (size_t)-1;
	return true;
}

bool Menu::onMouseMove(void)
{
	point pt = transformToWndCoord(m_system.getCursor().getPosition());
	pt -= m_area.getPosition();
	
	float item = pt.y / (2 * m_margin + m_itemHeight);
	m_selected = (size_t)item;
	
	if(m_selected > m_items.size())
		m_selected = (size_t)-1;

	invalidate();
	return true;
}

bool Menu::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
{
	if(m_selected < m_items.size())
	{
		MenuEventArgs m;
		m.name = "On_MenuClicked";
		m.mbutton = btn;
		m.mstate = state;
		m.id = (unsigned int)m_selected;
		callHandler(&m);
		return m.handled;
	}
	return true;
}


}