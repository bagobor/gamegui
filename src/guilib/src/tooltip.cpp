#include "StdAfx.h"
#include "tooltip.h"

namespace gui
{
	Tooltip::Tooltip(System& sys, const std::string& name) :
		base_window(sys, name),	
		m_fadein(0.f),
		m_fadeout(0.f)
	{
	}

	Tooltip::~Tooltip(void)
	{
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

	void Tooltip::rise()
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

	void Tooltip::reset(void)
	{
		setVisible(false);
		setIgnoreInputEvents(true);
	}

	void Tooltip::show()
	{
		rise();
		setVisible(true);
	}
}