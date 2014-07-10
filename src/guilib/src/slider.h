#pragma once

#include "button.h"

namespace gui
{
	class Slider : public base_window
	{
	public:
		typedef Slider Self;
		Slider(System& sys, const std::string& name = std::string());
		virtual ~Slider();

		static const char* GetType() { return "Slider"; }
		virtual const char* getType() const { return Self::GetType(); }

		float getScrollPosition() const;
		void setScrollPosition(float p);
		
		float getStepSize() const { return m_step; }
		void setStepSize(float p) { m_step = p; }

		float getDocumentSize() const;
		void setDocumentSize(float p);

		virtual bool onLoad();
		virtual bool onSized(bool update = true);
		virtual void init(xml::node& node);

		void onTrack(const events::TrackEvent& e);

	protected:
		virtual void updateThumb();

	protected:
		Thumb* m_thumb;
		float m_pos;
		float m_doc;
		float m_step;
	};

	class  ScrollBar : public Slider
	{
	public:
		typedef ScrollBar Self;
		ScrollBar(System& sys, const std::string& name = std::string());
		virtual ~ScrollBar();

		static const char* GetType() { return "ScrollBar"; }
		virtual const char* getType() const { return Self::GetType(); }

		virtual bool onLoad();
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onMouseWheel(int delta);
		virtual void init(xml::node& node);

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		void onIncClick(const events::ClickEvent& e);
		void onDecClick(const events::ClickEvent& e);

	protected:
		virtual void updateThumb();

	protected:
		Button* m_incBtn;
		Button* m_decBtn;

		const Image* m_backImg;
		const Image* m_leftImg;
		const Image* m_rightImg;

		ImagesetPtr m_imgset;

		bool m_horiz;
	};
}