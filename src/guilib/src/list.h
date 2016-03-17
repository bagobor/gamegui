
#pragma once

#include "panel.h"

namespace gui
{
	class BaseList : public Panel
	{
	public:
		typedef BaseList self_t;
		BaseList(System& sys, const std::string& name = std::string());
		virtual ~BaseList();

		static const char* GetType() { return "BaseList"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual void clear();

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		virtual void init(xml::node& node);
		virtual bool onLoad();

		void setColumns(unsigned int col) { m_columns = col > 0 ? col : 1; }
		unsigned int getColumns() const { return m_columns; }
		void setColumnWidth(float w) { m_colWidth = w; }
		float getColumnWidth() const { return m_colWidth; }

	protected:
		virtual void layoutItems();
		virtual void onChildAdd(window_ptr& node);
		virtual void onChildRemove(window_ptr& node);
		virtual bool onResumeLayout();

	protected:
		bool m_drawpanel;
		unsigned int m_columns;
		float m_colWidth;
		Rect m_borders;
	};

	class Label;
	class Font;
	typedef std::shared_ptr<Font> FontPtr;

	class ListBox : public BaseList
	{
	public:
		typedef ListBox self_t;
		ListBox(System& sys, const std::string& name = std::string());
		virtual ~ListBox(void);

		static const char* GetType() { return "ListBox"; }
		virtual const char* getType() const { return self_t::GetType(); }

		void addItem(const std::string& name);

		virtual bool onMouseMove(void);
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onMouseEnter(void);
		virtual bool onMouseLeave(void);

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		virtual void init(xml::node& node);

		Label* getSelectedItem() const;

	protected:
		virtual void onChildRemove(window_ptr& node);

	protected:
		FontPtr m_font;
		ImagesetPtr m_imgset;
		const Image* m_selection;
		window_ptr m_selectedItem;
	};

	class  CategorizedList : public BaseList
	{
	public:
		struct Category
		{
			Category(CategorizedList& p, const std::string& n) 
				: parent(p)
				, collapsed(true)
				, name(n)
			{}

			bool collapsed;
			std::string name;
			Rect area;
			Rect clickable;
			std::vector<window_ptr> children;
			CategorizedList& parent;

			void rename(const std::string& n);
			unsigned int add(WindowBase* wnd);
			void remove(unsigned int idx);
			WindowBase*	getWndByIndex(unsigned int idx);
			unsigned int getChildrenCount() const;
			void collapse() { fold(true); }
			void expand() { fold(false); }
			void fold(bool status);
			bool isCollapsed() const { return collapsed; }

			float calcHeight();
			void layoutChildren();

		private:
			Category& operator=(const Category& rhs) {rhs;}
		};

		typedef std::shared_ptr<Category> CategoryPtr;

		typedef CategorizedList self_t;
		CategorizedList(System& sys, const std::string& name = std::string());
		virtual ~CategorizedList(void);

		static const char* GetType() { return "CategorizedList"; }
		virtual const char* getType() const { return self_t::GetType(); }

		unsigned int addCategory(const std::string& name);
		Category* getCategoryByIndex(unsigned int idx);		
		unsigned int getCategoryCount() const;
		void removeCategory(unsigned int idx);
		virtual void Clear();
		FontPtr getFont() const { return m_font; }

		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		virtual void init(xml::node& node);

	protected:
		virtual void layoutItems();
		virtual void onChildAdd(window_ptr& node);
		virtual void onChildRemove(window_ptr& node);

	protected:
		FontPtr m_font;
		float m_indent;
		ImagesetPtr m_imgset;
		const Image* m_folded;
		const Image* m_unfolded;

		typedef std::vector<CategoryPtr> Categories;
		typedef Categories::iterator CategoriesIter;
		Categories m_categories;
	};
}
