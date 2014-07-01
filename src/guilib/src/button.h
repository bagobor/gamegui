#pragma once

#include "label.h"

namespace gui
{
	class Image;
	class Imageset;
	typedef std::shared_ptr<Imageset> ImagesetPtr;

	class Button : public Label
	{
	public:
		enum States
		{
			Normal,
			Hovered,
			Pushed,
			Disabled
		};

		struct StateImagery
		{
			//TODO: add color
			const Image* backImg;
			const Image* leftImg;
			const Image* rightImg;
		};

	public:
		typedef Button Self;
		Button(System& sys, const std::string& name = std::string());
		virtual ~Button(void);

		static const char* GetType() { return "Button"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual bool onMouseEnter(void);
		virtual bool onMouseLeave(void);
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);
		virtual bool onKeyboardButton(EventArgs::Keys key, EventArgs::ButtonState state);
		virtual bool onCaptureLost(void);

		virtual void init(xml::node& node);

		States getState() const { return m_state; }

	protected:
		States getStateByString(const std::string& type);

	protected:
		typedef std::map<unsigned int, StateImagery> StateMap; 
		StateMap m_states;
		
		ImagesetPtr m_imgset;

		States m_state;
	};

	class  ImageButton : public Button
	{
	public:
		typedef ImageButton Self;
		ImageButton(System& sys, const std::string& name = std::string());
		virtual ~ImageButton();

		static const char* GetType() { return "ImageButton"; }
		virtual const char* getType() { return Self::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void init(xml::node& node);

		void setStateImage(States state, const Image* image) { m_stateimg[state] = image; }
		const Image* getStateImage(States state) { return m_stateimg[state]; }

	protected:
		typedef std::map<unsigned int, const Image*> StateImages; 
		StateImages m_stateimg;
	};

	class  Thumb : public ImageButton
	{
	public:
		typedef Thumb Self;
		Thumb(System& sys, const std::string& name = std::string());
		virtual ~Thumb(void);

		static const char* GetType() { return "Thumb"; }
		virtual const char* getType() { return Self::GetType(); }

		float getProgress() const;
		void setProgress(float p);

		void setTrackarea(const Rect& rc);
		const Rect& getTrackarea() const { return m_trackarea; }

		virtual bool onMouseMove();
		virtual bool onMouseButton(EventArgs::MouseButtons btn, EventArgs::ButtonState state);

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void init(xml::node& node);

	protected:
		bool m_movable;
		bool m_horiz;
		point m_offset;
		Rect m_trackarea;
	};

	class  ScrollThumb : public Thumb
	{
	public:
		typedef ScrollThumb Self;
		ScrollThumb(System& sys, const std::string& name = std::string());
		virtual ~ScrollThumb(void);

		static const char* GetType() { return "ScrollThumb"; }
		virtual const char* getType() { return Self::GetType(); }
		
		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void init(xml::node& node);
	};
}