#include "stdafx.h"
#include "rect.h"

namespace gui
{
	/*************************************************************************
	Constructor
	*************************************************************************/
	Rect::Rect(float left, float top, float right, float bottom)
		: m_top(top)
		, m_bottom(bottom)
		, m_left(left)
		, m_right(right)
	{
	}

	Rect::Rect(const point& pos, const Size& sz)
		: m_top(pos.y)
		, m_bottom(pos.y + sz.height)
		, m_left(pos.x)
		, m_right(pos.x + sz.width)
	{
	}


	/*************************************************************************
	Return a Rect object that is the intersection of 'this' with 'rect'
	*************************************************************************/
	Rect Rect::getIntersection(const Rect& rect) const
	{
		// check for total exclusion
		if ((m_right > rect.m_left) &&
			(m_left < rect.m_right) &&
			(m_bottom > rect.m_top) &&
			(m_top < rect.m_bottom))
		{
			float left = (m_left > rect.m_left) ? m_left : rect.m_left;
			float right = (m_right < rect.m_right) ? m_right : rect.m_right;
			float top = (m_top > rect.m_top) ? m_top : rect.m_top;
			float bottom = (m_bottom < rect.m_bottom) ? m_bottom : rect.m_bottom;
			
			return Rect(left, top, right, bottom);
		}
		return Rect();
	}

	/*************************************************************************
	Apply an offset the the Rect
	*************************************************************************/
	Rect& Rect::offset(const point& pt)
	{
		m_left		+= pt.x;
		m_right		+= pt.x;
		m_top		+= pt.y;
		m_bottom	+= pt.y;
		return *this;
	}


	/*************************************************************************
	Check if a given point is within the Rect
	*************************************************************************/
	bool Rect::isPointInRect(const point& pt) const
	{
		if ((m_left > pt.x) ||
			(m_right <= pt.x) ||
			(m_top > pt.y) ||
			(m_bottom <= pt.y))
		{
			return false;
		}

		return true;
	}

	/*************************************************************************
	Set location of rect retaining current size.
	*************************************************************************/
	void Rect::setPosition(const point& pt)
	{
		Size sz(getSize());

		m_left = pt.x;
		m_top  = pt.y;
		setSize(sz);
	}


	/*************************************************************************
	check the size of the Rect object and if it is bigger than 'sz', 
	resize it so it isn't.	
	*************************************************************************/
	Rect& Rect::constrainSizeMax(const Size& sz)
	{
		if (getWidth() > sz.width)
		{
			setWidth(sz.width);
		}

		if (getHeight() > sz.height)
		{
			setHeight(sz.height);
		}

		return *this;
	}


	/*************************************************************************
	check the size of the Rect object and if it is smaller than 'sz',
	resize it so it isn't.
	*************************************************************************/
	Rect& Rect::constrainSizeMin(const Size& sz)
	{
		if (getWidth() < sz.width)
		{
			setWidth(sz.width);
		}

		if (getHeight() < sz.height)
		{
			setHeight(sz.height);
		}

		return *this;
	}


	/*************************************************************************
	check the size of the Rect object and if it is bigger than 'max_sz'
	or smaller than 'min_sz', resize it so it isn't.
	*************************************************************************/
	Rect& Rect::constrainSize(const Size& max_sz, const Size& min_sz)
	{
		Size curr_sz(getSize());

		if (curr_sz.width > max_sz.width)
		{
			setWidth(max_sz.width);
		}
		else if (curr_sz.width < min_sz.width)
		{
			setWidth(min_sz.width);
		}

		if (curr_sz.height > max_sz.height)
		{
			setHeight(max_sz.height);
		}
		else if (curr_sz.height < min_sz.height)
		{
			setHeight(min_sz.height);
		}

		return *this;
	}

	Rect& Rect::scale(const Size& size)
	{
		const float scaleX = 1 / size.width;
		const float scaleY = 1 / size.height;
		m_left *= scaleX;
		m_right *= scaleX;
		m_top *= scaleY;
		m_bottom *= scaleY;

		return *this;
	}
    
    Rect& Rect::scale_u(float scalar)
	{
        this->operator *= (scalar);
		return *this;
	}

}
