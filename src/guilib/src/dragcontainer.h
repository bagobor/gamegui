#pragma once
#include "window.h"

namespace gui
{
	class  DragContainer : public base_window
	{
	public:
		typedef DragContainer Self;
		DragContainer(System& sys, const std::string& name = std::string());
		virtual ~DragContainer(void);

		static const char* GetType() { return "DragContainer"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void rise();

		void update(base_window* target, const point& pt);
		
		bool startDrag(base_window* subj, const point& off);
		bool stopDrag(void);

		void reset(void);


	protected:
		base_window* m_dragSubject;
		
		base_window* m_dropTarget;
		point		m_childOffset;
		
	};
}