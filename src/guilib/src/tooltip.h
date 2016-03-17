#pragma once
#include "window.h"

namespace gui
{
	class  Tooltip : public WindowBase
	{
	public:
		typedef Tooltip self_t;
		Tooltip(System& sys, const std::string& name = std::string());
		virtual ~Tooltip(void);

		static const char* GetType() { return "Tooltip"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual void rise();

		void reset(void);

		void show();

		float getFadeIn() const { return m_fadein; }
		float getFadeOut() const { return m_fadeout; }
		
		void setFadeIn(float f) { m_fadein = f; }
		void setFadeOut(float f) { m_fadeout = f; }

	protected:		
		float	m_fadein;
		float	m_fadeout;
	};
}