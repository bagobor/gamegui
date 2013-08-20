#pragma once

#include "vector.h"
#include "size.h"

namespace gui
{
	/*!
	\brief
	Class encapsulating operations on a Rectangle
	*/
	class  Rect
	{
	public:
		Rect() : m_left(0), m_top(0), m_right(0), m_bottom(0) {}
		Rect(float left, float top, float right, float bottom);
		Rect(point pos, Size sz);

		point	getPosition() const;
		float	getWidth() const;
		float	getHeight() const;
		Size	getSize() const;

		void	setPosition(const point& pt);
		void	setWidth(float width);
		void	setHeight(float height);
		void	setSize(const Size& size);

		Rect	getIntersection(const Rect& rect) const;
		Rect&	offset(const point& pt);
		bool	isPointInRect(const point& pt) const;
		Rect&	constrainSizeMax(const Size& sz);
		Rect&	constrainSizeMin(const Size& sz);

		Rect&	constrainSize(const Size& max_sz, const Size& min_sz);

		Rect&	scale(const Size& size);
		bool	empty(void) const;

		bool	operator==(const Rect& rhs) const;
		bool	operator!=(const Rect& rhs) const;
		Rect operator*(float scalar) const;
		const Rect& operator*=(float scalar);
		const Rect& operator*=(const Size& sz);

		float	m_left, m_top, m_right, m_bottom;
	};

	inline point Rect::getPosition() const
	{
		return point(m_left, m_top);
	}
	inline float Rect::getWidth() const
	{
		return m_right - m_left;
	}
	inline float Rect::getHeight() const
	{
		return m_bottom - m_top;
	}
	inline Size Rect::getSize() const
	{
		return Size(getWidth(), getHeight());
	}
	inline void Rect::setWidth(float width)
	{
		m_right = m_left + width;
	}
	inline void Rect::setHeight(float height)
	{
		m_bottom = m_top + height;
	}
	inline void Rect::setSize(const Size& size)
	{
		setWidth(size.width);
		setHeight(size.height);
	}
	inline bool Rect::empty(void) const
	{
		return !(getWidth() > FLT_EPSILON && getHeight() > FLT_EPSILON);
	}
	inline bool	Rect::operator==(const Rect& rhs) const
	{
		return ((m_left == rhs.m_left) && (m_right == rhs.m_right) && (m_top == rhs.m_top) && (m_bottom == rhs.m_bottom));
	}
	inline bool	Rect::operator!=(const Rect& rhs) const
	{
		return !operator==(rhs);
	}
	inline Rect Rect::operator*(float scalar) const
	{
		return Rect(m_left * scalar, m_top * scalar, m_right * scalar, m_bottom * scalar);
	}
	inline const Rect& Rect::operator*=(float scalar)
	{
		m_left *= scalar;
		m_top *= scalar;
		m_right *= scalar;
		m_bottom *= scalar;
		return *this;
	}
	inline const Rect& Rect::operator*=(const Size& sz)
	{
		return  scale(sz);
	}
}
