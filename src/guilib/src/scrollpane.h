#pragma once

#include "window.h"

namespace gui
{
	class ScrollBar;
	class ScrollPane;

	class ScrollPane : public WindowBase
	{
	public:
		typedef ScrollPane self_t;
		ScrollPane(System& sys, const std::string& name = std::string());
		virtual ~ScrollPane();

		static const char* GetType() { return "ScrollPane"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual bool onLoad();
		virtual bool onSized(bool update = true);
		virtual bool onMouseWheel(int delta);
		virtual void init(xml::node& node);

		void setTarget(WindowBase* target);

		void onTrack(const events::TrackEvent& e);
		void onTargetSized(const events::SizedEvent& e);

	protected:
		void layoutTarget();

	protected:
		ScrollBar*	m_scroll;
		WindowBase* m_target;
	};
}