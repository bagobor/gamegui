#include "StdAfx.h"

#include "combobox.h"

#include "system.h"
#include "list.h"
#include "eventtypes.h"

namespace gui
{
	Combobox::Combobox(System& sys, const std::string& name) :
		Editbox(sys, name)
	{
	}

	Combobox::~Combobox(void)
	{
	}

	void Combobox::AddItem(const std::string& name)
	{
		if(m_list)
		{
			ListBox* lb = static_cast<ListBox*>(m_list.get());
			if(lb)
			{
				lb->addItem(name);
			}
		}
	}

	bool Combobox::onLoad(void)
	{
		m_btn = child("_Btn");
		if(m_btn)
		{
			m_btn->setAlwaysOnTop(true);
			moveToFront(m_btn.get());
			subscribe<events::ClickEvent, Combobox> (&Combobox::onBtnClick, m_btn.get());
		}
		m_list = child("_List");
		if(m_list)
		{
			m_list->setAlwaysOnTop(true);
			m_list->setVisible(false);
			subscribe<events::ClickEvent, Combobox> (&Combobox::onListClick, m_list.get());
		}
		return Editbox::onLoad();
	}

	void Combobox::onBtnClick(const events::ClickEvent& e) 
	{
		if(m_list)
		{
			if (m_list->getVisible())
			{
				hideList();
			}
			else
			{
				m_system.getRootWindow().addChildWindow(m_list.get());
				m_list->setSize(Size(m_area.getSize().width, m_list->getSize().height));
				m_list->setVisible(true);
				m_list->moveToFront();
				point pt = transformToRootCoord(point(m_area.m_left, m_area.m_bottom));
				m_list->setPosition(pt);
				setInputFocus(true);
			}
		}
	}

	void Combobox::onListClick(const events::ClickEvent& e) 
	{
		static const std::string empty_string;
		if(m_list)
		{
			hideList();

			ListBox* lb = static_cast<ListBox*>(m_list.get());
			if(lb)
			{
				Label* st = lb->getSelectedItem();
				setText(st ? st->getText() : empty_string);
			}
			else
			{
				setText(empty_string);
			}

		}
	}

	bool Combobox::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		hideList();

		return Editbox::onMouseButton(btn, state);
	}

	bool Combobox::onFocusLost(WindowBase* newFocus)
	{
		if ((!newFocus) || ((newFocus != m_btn.get()) && (newFocus != m_list.get())))
			hideList();

		return Editbox::onFocusLost(newFocus);
	}

	void Combobox::hideList()
	{
		if (m_list && m_list->getVisible())
		{
			addChildWindow(m_list.get());
			m_list->setVisible(false);
		}
	}

}
