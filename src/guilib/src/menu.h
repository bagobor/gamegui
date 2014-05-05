#pragma once
#include "panel.h"

namespace gui
{
	class Font;
	typedef std::shared_ptr<Font> FontPtr;

	struct  MenuItem
	{
		std::string text;
		const Image* icon;
		Color col;
		Color selcol;

		MenuItem() : icon(0) {}
		MenuItem(const MenuItem& item) : text(item.text), icon(item.icon), col(item.col), selcol(item.selcol) {}	
	};

	class  Menu : public Panel
	{
	public:
		typedef Menu Self;
		Menu(System& sys, const std::string& name = std::string());
		virtual ~Menu(void);

		static const char* GetType() { return "Menu"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void rise();
		void setFont(const std::string& font);

		void reset(void);

		void show(point& pt);

		float getFadeIn() const { return m_fadein; }
		float getFadeOut() const { return m_fadeout; }
		
		void setFadeIn(float f) { m_fadein = f; }
		void setFadeOut(float f) { m_fadeout = f; }

		void		addItem(MenuItem item);
		MenuItem*	getItem(std::string text);
		MenuItem*	getItem(size_t id);
		void		clear();

		virtual void init(xml::node& node);
		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual bool onMouseMove(void);
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onMouseEnter(void);
		virtual bool onMouseLeave(void);

	protected:

		FontPtr m_font;
		const Image* m_selImg;
		size_t m_selected;

		typedef std::vector<MenuItem> MenuItemVector;
		MenuItemVector	m_items;
		
		float	m_fadein;
		float	m_fadeout;

		float	m_itemHeight;
		float	m_maxWidth;
		float	m_minWidth;
		float	m_margin;
		float	m_actualWidth;

		struct itemfinder
		{
			std::string txt;
			itemfinder(std::string t) : txt(t) {}
			
			bool operator()(const MenuItem& item)
			{
				return (item.text == txt);
			}
		};
	};
}