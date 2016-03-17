#pragma once
#include "window.h"

namespace gui
{
	class Image;
	class Imageset;
	typedef std::shared_ptr<Imageset> ImagesetPtr;

	class Panel : public WindowBase
	{
	public:
		typedef Panel self_t;
		Panel(System& sys, const std::string& name = std::string());
		virtual ~Panel(void);

		static const char* GetType() { return "Panel"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);
		virtual void init(xml::node& node);

	protected:
		void renderFrame(const Rect& dest, const Rect& clip);

	protected:
		ImagesetPtr m_imgset;

		// frame imagery
		const Image*	m_backImg;
		const Image*	m_topImg;
		const Image*	m_toplImg;
		const Image*	m_toprImg;
		const Image*	m_lImg;
		const Image*	m_rImg;
		const Image*	m_botImg;
		const Image*	m_botlImg;
		const Image*	m_botrImg;
	};
}