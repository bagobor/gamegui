#pragma once

#include "editbox.h"

namespace gui
{
	class Combobox : public Editbox
	{
	public:
		typedef Combobox self_t;

		Combobox(System& sys, const std::string& name = std::string());
		virtual ~Combobox();

		static const char* GetType() { return "Combobox"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual bool onLoad();
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onFocusLost(WindowBase* newFocus);
		void onBtnClick(const events::ClickEvent& e);
		void onListClick(const events::ClickEvent& e);

		void AddItem(const std::string& name);

	private:
		void hideList();

	protected:
		window_ptr m_btn;
		window_ptr m_list;
	};
}
