#pragma once
#include "window.h"

namespace gui
{
	class DragContainer : public WindowBase
	{
	public:
		typedef DragContainer self_t;
		DragContainer(System& sys, const std::string& name = std::string());
		virtual ~DragContainer(void);

		static const char* GetType() { return "DragContainer"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual void rise();

		void update(WindowBase* target, const point& pt);
		
		bool startDrag(WindowBase* subj, const point& off);
		bool stopDrag(void);

		void reset(void);


	protected:
		WindowBase* m_dragSubject;
		
		WindowBase* m_dropTarget;
		point		m_childOffset;
		
	};
}