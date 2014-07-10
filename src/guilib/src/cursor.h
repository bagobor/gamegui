#pragma once

#include "vector.h"


namespace gui
{
	class Imageset;
	typedef std::shared_ptr<Imageset> ImagesetPtr;

	class Image;
	class Renderer;

	class  Cursor
	{
	public:
		Cursor(Renderer& r);
		~Cursor();

		void setImageset(ImagesetPtr set) { m_cursors = set; }
		void setType(std::string curname);
		std::string getType() const;
		void render();

		void setPosition(point pt);
		point getPosition() const;

		bool isVisible() const { return m_visible; }
		void setVisible(bool visible)  { m_visible = visible; }
		void hide() { m_visible = false; }
		void show() { m_visible = true; }

	protected:
		Cursor& operator=(const Cursor&) { return *this; }
		bool			m_visible;
		ImagesetPtr		m_cursors;	
		point			m_cursorPos;
		point			m_renderPos;
		const Image*	m_currentCursor;
		Renderer&		m_render;
	};
}
