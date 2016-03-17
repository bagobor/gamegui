#pragma once
#include "label.h"

namespace gui
{
	class Image;
	class Imageset;
	typedef std::shared_ptr<Imageset> ImagesetPtr;

	class  Checkbox : public Label
	{
	public:
		typedef Checkbox self_t;
		Checkbox(System& sys, const std::string& name = std::string());
		virtual ~Checkbox();
		
		static const char* GetType() { return "Checkbox"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		bool isChecked() const { return m_checked; }
		void setChecked(bool check);

		virtual bool onMouseEnter();
		virtual bool onMouseLeave();
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state);

		virtual void init(xml::node& node);

	protected:
		bool m_checked;
		bool m_hovered;
		bool m_pushed;

		ImagesetPtr m_imgset;
		const Image* m_imgUnchecked;
		const Image* m_imgChecked;
		const Image* m_imgUncheckedHovered;
		const Image* m_imgCheckedHovered;
	};
}