#include "StdAfx.h"
#include "checkbox.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"
#include "utils.h"

namespace gui
{
	Checkbox::Checkbox(System& sys, const std::string& name):
		Label(sys, name),
		m_imgChecked(0),
		m_imgUnchecked(0),
		m_checked(false),
		m_hovered(false)
	{
	}

	Checkbox::~Checkbox(void)
	{
	}

	bool Checkbox::onMouseEnter(void)
	{
		m_hovered = true;
		invalidate();
		return true;
	}

	bool Checkbox::onMouseLeave(void)
	{
		m_hovered = false;
		invalidate();
		return true;
	}

	bool Checkbox::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		if(state == EventArgs::Down)
		{
			m_system.queryCaptureInput(this);
		}
		else
		{
			m_system.queryCaptureInput(0);
			m_checked = !m_checked;
			EventArgs a;
			a.name = "On_StateChanged";
			callHandler(&a);
		}
		invalidate();
		return true;
	}

	bool Checkbox::onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state)
	{	
		switch(key)
		{
		case EventArgs::K_SPACE:
		case EventArgs::K_RETURN:
		case EventArgs::K_EXECUTE:
			if(state == EventArgs::Down)
			{
				//m_system.queryCaptureInput(this);
			}
			else
			{
				//m_system.queryCaptureInput(0);
				m_checked = !m_checked;
				EventArgs a;
				a.name = "On_StateChanged";
				callHandler(&a);
				invalidate();
			}
			return true;
			break;
		};
		return false;
	}

	void Checkbox::render(const Rect& finalRect, const Rect& finalClip)
	{
		Renderer& r = m_system.getRenderer();

		const Image* state = m_checked ? m_imgChecked : m_imgUnchecked;
		float offset = 0.f;
		if(state)
		{
			Size imgSize = state->GetSize();
			Rect dest(finalRect);
			dest.setWidth(imgSize.width);
			offset += imgSize.width;
			r.draw(*state, dest, 1.f, finalClip,  m_backColor, Stretch, Stretch);
		}
		
		Rect desttext(finalRect);
		desttext.m_left += offset + 4.f;
		Color result = m_hovered || m_focus ? Color(1.f, 0.7f, 0.7f) : m_backColor;

		Label::render(desttext, finalClip);	
	}

	void Checkbox::init(xml::node& node)
	{
		Label::init(node);

		xml::node setting = node("Checked");
		if(!setting.empty())
		{
			m_checked = StringToBool(setting.first_child().value());
		}

		xml::node state = node("StateImagery");
		if(!state.empty())
		{
			std::string setname = state["Imageset"].value();
			m_imgset = m_system.getWindowManager().loadImageset(setname);			
			if(m_imgset)
			{
				const Imageset& set = *m_imgset;
				m_imgChecked = set[state("Checked")["Image"].value()];
				m_imgUnchecked = set[state("Unchecked")["Image"].value()];
			}
		}
	}
}
