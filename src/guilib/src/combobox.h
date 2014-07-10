#pragma once

#include "editbox.h"

namespace gui
{
	class Combobox : public Editbox
	{
	public:
		typedef Combobox Self;
		Combobox(System& sys, const std::string& name = std::string());
		virtual ~Combobox();

		static const char* GetType() { return "Combobox"; }
		virtual const char* getType() const { return Self::GetType(); }

		virtual bool onLoad();
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onFocusLost(base_window* newFocus);
		void onBtnClick(const events::ClickEvent& e);
		void onListClick(const events::ClickEvent& e);

		void AddItem(const std::string& name);

	protected:
		window_ptr m_btn;
		window_ptr m_list;

	private:
		void hideList()
		{
			if (m_list && m_list->getVisible())
			{
				addChildWindow(m_list.get());
				m_list->setVisible(false);
			}
		}
	};
}
