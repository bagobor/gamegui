#include "stdafx.h"

#include "list.h"

#include "system.h"
#include "windowmanager.h"
#include "renderer.h"
#include "font.h"
#include "utils.h"
#include "label.h"
#include "eventtypes.h"

namespace gui
{
	namespace detail
	{
		struct HitTestTask
		{
			point pt;
			HitTestTask(point p) : pt(p) {}
			bool operator () (window_ptr w) { return w ? w->getArea().isPointInRect(pt) : false; }
			bool operator () (CategorizedList::CategoryPtr cat) { return cat ? cat->clickable.isPointInRect(pt) : false; }
		};
	}

	struct VisibleTask
	{
		bool visible;
		VisibleTask(bool v) : visible(v) {}
		void operator () (window_ptr w) { if (w) w->setVisible(visible); }
	};

	struct HeightCalcTask
	{
		float& height;
		HeightCalcTask(float& h) : height(h) {}
		void operator () (window_ptr w) { if (w) height += w->getArea().getHeight(); }
		HeightCalcTask& operator=(const HeightCalcTask& rhs) { rhs; }
	};

	BaseList::BaseList(System& sys, const std::string& name) 
	: Panel(sys, name)
	, m_drawpanel(false)
	, m_columns(1)
	, m_colWidth(100.f)
	, m_borders(0.f, 0.f, 0.f, 0.f)
	{
	}

	BaseList::~BaseList(void)
	{
	}

	bool BaseList::onLoad(void)
	{
		layoutItems();
		return WindowBase::onLoad();
	}

	void BaseList::clear()
	{
		m_children.clear();
		invalidate();
	}

	void BaseList::onChildAdd(window_ptr& node)
	{
		layoutItems();
	}

	void BaseList::onChildRemove(window_ptr& node)
	{	
		layoutItems();
	}

	bool BaseList::onResumeLayout(void)
	{
		layoutItems();
		return WindowBase::onResumeLayout();
	}

	void BaseList::layoutItems()
	{
		if(m_suspended)
			return;

		invalidate();
		Size sz(m_area.getWidth(), m_borders.m_top);
		child_iter i = m_children.begin();
		child_iter end = m_children.end();
		while(i != end)
		{
			float height = 0.f;
			float width = m_borders.m_left;
			for(unsigned int col = 0; col < m_columns; col++)
			{
				if(i == end)
					break;
				Size child((*i)->getSize());
				(*i)->setPosition(point(m_borders.m_left + col * m_colWidth, sz.height));
				height = height > child.height ? height : child.height;
				width += child.width;
				++i;
			}
			
			sz.height += height;
			float widthWithBorders = width + m_borders.m_right;
			sz.width = widthWithBorders > sz.width ? widthWithBorders : sz.width;
		}
		sz.height += m_borders.m_bottom;
		setSize(sz);
	}

	void BaseList::render(const Rect& finalRect, const Rect& finalClip)
	{
		if(m_drawpanel)
			renderFrame(finalRect, finalClip);
	}

	void BaseList::init(xml::node& node)
	{
		Panel::init(node);

		xml::node setting = node("HasFrame");
		if(!setting.empty())
		{
			m_drawpanel = StringToBool(setting.first_child().value());
		}
		setting = node("Columns");
		if(!setting.empty())
		{
			m_columns = (unsigned int)atoi(setting.first_child().value());
			if(!m_columns)
				m_columns = 1;
		}
		setting = node("ColumnWidth");
		if(!setting.empty())
		{
			m_colWidth = (float)atof(setting.first_child().value());
		}
		setting = node("Borders");
		if(!setting.empty())
		{
			m_borders = StringToRect(setting.first_child().value());
		}
	}

	ListBox::ListBox(System& sys, const std::string& name) 
	: BaseList(sys, name)
	, m_selection(0)
	{
	}

	ListBox::~ListBox(void)
	{
	}

	void ListBox::addItem(const std::string& name)
	{
		if(m_font && !name.empty())
		{
			float w = m_area.getWidth() - m_borders.m_left - m_borders.m_right;
			float h = m_font->getLineSpacing();
			
			Label* st = new Label(m_system);
			st->setSize(Size(w, h));

			addChildWindow(st);
			st->setFont(m_font);
			st->setText(name);
			st->setIgnoreInputEvents(true);
			st->setAlwaysOnTop(true);
			st->setVisible(true);
		}
	}

	Label* ListBox::getSelectedItem() const
	{
		return m_selectedItem ? static_cast<Label*>(m_selectedItem.get()) : 0;
	}

	void ListBox::render(const Rect& finalRect, const Rect& finalClip)
	{
		BaseList::render(finalRect, finalClip);

		if(m_selection && m_selectedItem)
		{
			Rect rc(m_selectedItem->getArea());
			rc.offset(finalRect.getPosition());

			Renderer& r = m_system.getRenderer();
			r.draw(*m_selection, rc, 1.f, finalClip, m_backColor, ImageOps::Tile, ImageOps::Stretch);
		}
	}

	bool ListBox::onMouseEnter(void)
	{
		m_selectedItem = 0;
		invalidate();
		return true;
	}

	bool ListBox::onMouseLeave(void)
	{
		m_selectedItem = 0;
		invalidate();
		return true;
	}

	bool ListBox::onMouseMove(void)
	{
		point pt = transformToWndCoord(m_system.getCursor().getPosition()) - m_area.getPosition();
		child_iter i = std::find_if(m_children.begin(), m_children.end(), detail::HitTestTask(pt));
		if(i != m_children.end())
		{
			if(m_selectedItem == *i)
				return true;
			m_selectedItem = *i;
		}
		else
		{
			// shouldn't be here
			m_selectedItem = 0;
		}
		invalidate();
		return true;
	}

	bool ListBox::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		if(state == EventArgs::Up)
		{
			send(events::ClickEvent());
		}
		return true;
	}

	void ListBox::onChildRemove(window_ptr& node)
	{
		if(m_selectedItem == node)
			m_selectedItem = 0;
		BaseList::onChildRemove(node);
	}

	void ListBox::init(xml::node& node)
	{
		BaseList::init(node);

		xml::node setting = node("Font");
		if(!setting.empty())
		{
			m_font = m_system.getWindowManager().loadFont(setting.first_child().value());
		}
		
		xml::node frame = node("SelectImagery");
		if(!frame.empty())
		{
			m_imgset = m_system.getWindowManager().loadImageset(frame["imageset"].value());
			if(m_imgset)
			{
				const Imageset& set = *m_imgset;
				m_selection = set[frame["Image"].value()];
			}
		}
	}

	CategorizedList::CategorizedList(System& sys, const std::string& name) 
	: BaseList(sys, name)
	, m_folded(0)
	, m_unfolded(0)
	, m_indent(0.f)
	{
	}

	CategorizedList::~CategorizedList(void)
	{
	}

	void CategorizedList::onChildAdd(window_ptr& node)
	{
	}

	void CategorizedList::onChildRemove(window_ptr& node)
	{
	}

	void CategorizedList::layoutItems()
	{
		invalidate();

		if(!m_font)
			return;

		float pos = 0.f;
		CategoriesIter i = m_categories.begin();
		CategoriesIter end = m_categories.end();
		while(i != end)
		{
			CategoryPtr p = *i;
			float size = m_font->getLineSpacing();
			p->clickable.m_left = 0.f;
			p->clickable.m_right = m_area.getWidth();
			p->clickable.m_top = pos;
			p->clickable.m_bottom = pos + size;

			if(!p->collapsed)
			{
				size += p->calcHeight();
			}
			p->area.m_top = pos;
			p->area.m_left = m_indent;
			p->area.setSize(Size(m_area.getWidth(), size));

			p->layoutChildren();

			pos += size;

			++i;
		}
		setSize(Size(m_area.getWidth(), pos));
	}

	void CategorizedList::render(const Rect& finalRect, const Rect& finalClip)
	{
		BaseList::render(finalRect, finalClip);

		if(m_font)
		{
			Rect rc(finalRect);
			Renderer& r = m_system.getRenderer();

			CategoriesIter i = m_categories.begin();
			CategoriesIter end = m_categories.end();
			while(i != end)
			{
				CategoryPtr p = *i;

				Rect s(rc);
				s.m_left += p->area.m_left;
				s.m_top += p->area.m_top;

				m_font->drawText(p->name, s, 1.0f, finalClip, LeftAligned, m_backColor, 1.f, 1.f);

				s.m_left = finalRect.m_left;
				s.setSize(Size(m_indent, m_indent));

				const Image* img = p->collapsed ? m_folded : m_unfolded;
				if(img)
					r.draw(*img, s, 1.f, finalClip, m_backColor, ImageOps::Stretch, ImageOps::Stretch);

				++i;
			}
		}
	}

	void CategorizedList::init(xml::node& node)
	{
		BaseList::init(node);

		xml::node setting = node("Font");
		if(!setting.empty())
		{
			m_font = m_system.getWindowManager().loadFont(setting.first_child().value());
		}

		setting = node("Indent");
		if(!setting.empty())
		{
			m_indent = (float)atof(setting.first_child().value());
		}
		
		xml::node frame = node("Folding");
		if(!frame.empty())
		{
			std::string setname = frame["imageset"].value();
			m_imgset = m_system.getWindowManager().loadImageset(setname);
			if(m_imgset)
			{
				const Imageset& set = *m_imgset;
				m_folded = set[frame("Folded")["Image"].value()];
				m_unfolded = set[frame("Unfolded")["Image"].value()];
			}
		}
	}

	unsigned int CategorizedList::addCategory(const std::string& name)
	{
		CategoryPtr p(new Category(*this, name));
		m_categories.push_back(p);
		layoutItems();
		return (unsigned int)m_categories.size() - 1;
	}

	CategorizedList::Category* CategorizedList::getCategoryByIndex(unsigned int idx)
	{
		if(idx >= m_categories.size())
			return NULL;

		return m_categories[idx].get();
	}

	unsigned int CategorizedList::getCategoryCount() const
	{
		return (unsigned int)m_categories.size();
	}

	void CategorizedList::removeCategory(unsigned int idx)
	{
		m_categories.erase(m_categories.begin() + idx);
		layoutItems();
	}

	void CategorizedList::Clear()
	{
		m_categories.clear();
		layoutItems();
	}


	void CategorizedList::Category::rename(const std::string& n)
	{
		name = n;
		parent.invalidate();
	}

	unsigned int CategorizedList::Category::add(WindowBase* wnd)
	{
		children.push_back(window_ptr(wnd));
		if(collapsed)
			wnd->setVisible(false);
		
		parent.addChildWindow(wnd);
		parent.layoutItems();

		return (unsigned int)children.size() - 1;
	}

	void CategorizedList::Category::remove(unsigned int idx)
	{
		if(idx >= children.size())
			return;
		
		parent.remove(children[idx]);
		children.erase(children.begin() + idx);
		parent.layoutItems();
	}

	WindowBase* CategorizedList::Category::getWndByIndex(unsigned int idx)
	{
		if(idx >= children.size())
			return NULL;

		return children[idx].get();
	}

	unsigned int CategorizedList::Category::getChildrenCount() const
	{
		return static_cast<unsigned int>(children.size());
	}

	void CategorizedList::Category::fold(bool status)
	{
		collapsed = status;
		std::for_each(children.begin(), children.end(), VisibleTask(!status));
		parent.layoutItems();
	}

	float CategorizedList::Category::calcHeight()
	{
		float height = 0.f;
		HeightCalcTask task(height);
		std::for_each(children.begin(), children.end(), task);
		return task.height;
	}

	void CategorizedList::Category::layoutChildren()
	{
		FontPtr font = parent.getFont();
		float pos = font ? font->getLineSpacing() : 0.f;
		std::vector<window_ptr>::iterator i = children.begin();
		std::vector<window_ptr>::iterator end = children.end();
		while(i != end)
		{
			window_ptr child = *i;
			child->setPosition(point(area.m_left, area.m_top + pos));
			pos += child->getArea().getHeight();

			++i;
		}
	}

	bool CategorizedList::onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state)
	{
		if(state == EventArgs::Up)
		{
			point pt = transformToWndCoord(m_system.getCursor().getPosition()) - m_area.getPosition();
			CategoriesIter i = std::find_if(m_categories.begin(), m_categories.end(), detail::HitTestTask(pt));
			if(i != m_categories.end())
			{
				CategoryPtr p = *i;
				p->fold(!p->collapsed);
			}
		}
		return true;
	}
}
