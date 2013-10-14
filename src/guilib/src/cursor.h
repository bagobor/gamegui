#pragma once

#include "vector.h"


namespace gui
{
	class Imageset;
	typedef boost::shared_ptr<Imageset> ImagesetPtr;

	class Image;
	class Renderer;

	class  Cursor
	{
	public:
		Cursor(Renderer& r);
		~Cursor();

		void setImageset(ImagesetPtr set) { m_cursors = set; }
		void setType(const std::string& curname);
		std::string getType(void) const;
		void render();

		void setPosition(point pt);
		point getPosition() const;

	protected:
		Cursor& operator=(const Cursor&) { return *this; }
		ImagesetPtr		m_cursors;	
		point			m_cursorPos;
		point			m_renderPos;
		const Image*	m_currentCursor;
		Renderer&		m_render;
	};
}
