#include "StdAfx.h"
#include "tooltip.h"

namespace gui
{
	Tooltip::Tooltip(System& sys, const std::string& name) :
		WindowBase(sys, name),	
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
			const WindowBase* m_ptr;
			seeker(const WindowBase* ptr) : m_ptr(ptr){}
			bool operator()(window_ptr obj) 
			{
				return obj ? (obj.get() == m_ptr) : false;
			}
		};
	}

	void Tooltip::rise()
	{
		if (getDisableRise() || !m_parent) return;

		children_t& children = m_parent->getChildren();
		if (children.size() < 2) return;
		
		for (child_iter it = children.begin(), end = children.end(); it != end; ++it)
		{
			if (it->get() != this) continue;
			std::swap(*(--children.end()), *it);
			return;
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