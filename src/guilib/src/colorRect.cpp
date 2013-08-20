#include "stdafx.h"
#include "colorRect.h"


namespace gui
{
/*************************************************************************
	Constructor
*************************************************************************/
ColorRect::ColorRect(const Color& top_left, const Color& top_right, const Color& bottom_left, const Color& bottom_right) :
	m_top_left(top_left),
	m_top_right(top_right),
	m_bottom_left(bottom_left),
	m_bottom_right(bottom_right)
{
}


/*************************************************************************
	Constructor for ColorRect objects (via single color).
*************************************************************************/
ColorRect::ColorRect(const Color& col) :
	m_top_left(col),
	m_top_right(col),
	m_bottom_left(col),
	m_bottom_right(col)
{
}


/*************************************************************************
	Default constructor
*************************************************************************/
ColorRect::ColorRect(void) :
	m_top_left(),
	m_top_right(),
	m_bottom_left(),
	m_bottom_right()
{
}


/*************************************************************************
	Set the alpha value to use for all four corners of the ColorRect.	
*************************************************************************/
void ColorRect::setAlpha(float alpha)
{
	m_top_left.setAlpha(alpha);
	m_top_right.setAlpha(alpha);
	m_bottom_left.setAlpha(alpha);
	m_bottom_right.setAlpha(alpha);
}


/*************************************************************************
	Set the alpha value to use for the top edge of the ColorRect.	
*************************************************************************/
void ColorRect::setTopAlpha(float alpha)
{
	m_top_left.setAlpha(alpha);
	m_top_right.setAlpha(alpha);
}


/*************************************************************************
	Set the alpha value to use for the bottom edge of the ColorRect.	
*************************************************************************/
void ColorRect::setBottomAlpha(float alpha)
{
	m_bottom_left.setAlpha(alpha);
	m_bottom_right.setAlpha(alpha);
}


/*************************************************************************
	Set the alpha value to use for the left edge of the ColorRect.	
*************************************************************************/
void ColorRect::setLeftAlpha(float alpha)
{
	m_top_left.setAlpha(alpha);
	m_bottom_left.setAlpha(alpha);
}


/*************************************************************************
	Set the alpha value to use for the right edge of the ColorRect.	
*************************************************************************/
void ColorRect::setRightAlpha(float alpha)
{
	m_top_right.setAlpha(alpha);
	m_bottom_right.setAlpha(alpha);
}

/*************************************************************************
	Determinate whehter the ColorRect is monochromatic or variegated
*************************************************************************/
bool ColorRect::isMonochromatic() const
{
	return m_top_left == m_top_right &&
		   m_top_left == m_bottom_left &&
		   m_top_left == m_bottom_right;
}

/*************************************************************************
	Get the color at a specified point
*************************************************************************/
Color ColorRect::getColorAtPoint( float x, float y ) const
{
    Color h1((m_top_right - m_top_left) * x + m_top_left);
	Color h2((m_bottom_right - m_bottom_left) * x + m_bottom_left);
	return Color((h2 - h1) * y + h1);
}

/*************************************************************************
	Get a ColorRectangle from the specified Region
*************************************************************************/
ColorRect ColorRect::getSubRectangle( float left, float right, float top, float bottom ) const
{
	return ColorRect(
		getColorAtPoint(left, top),
		getColorAtPoint(right, top),
		getColorAtPoint(left, bottom),
		getColorAtPoint(right, bottom)
	);
}


/*************************************************************************
	Set the color of all four corners simultaneously.	
*************************************************************************/
void ColorRect::setColors(const Color& col)
{
	m_top_left = m_top_right = m_bottom_left = m_bottom_right = col;
}


/*************************************************************************
	Module the alpha components of each corner's color by a constant.
*************************************************************************/
void ColorRect::modulateAlpha(float alpha)
{
	m_top_left.setAlpha(m_top_left.getAlpha()*alpha);
	m_top_right.setAlpha(m_top_right.getAlpha()*alpha);
	m_bottom_left.setAlpha(m_bottom_left.getAlpha()*alpha);
	m_bottom_right.setAlpha(m_bottom_right.getAlpha()*alpha);
}

/*************************************************************************
    Modulate all components of this color rect with corresponding
    components from another color rect.
*************************************************************************/
ColorRect& ColorRect::operator *=(const ColorRect& other)
{
    m_top_left *= other.m_top_left;
    m_top_right *= other.m_top_right;
    m_bottom_left *= other.m_bottom_left;
    m_bottom_right *= other.m_bottom_right;

    return *this;
}

}