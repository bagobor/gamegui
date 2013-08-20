#pragma once

namespace gui
{
	/*!
	\brief
	Class that holds the size (width & height) of something.
	*/
	class  Size
	{
	public:
		Size(float width = 0, float height = 0) : width(width), height(height) {}

		bool operator==(const Size& other) const;
		bool operator!=(const Size& other) const;

		float width, height;
	};

	inline bool Size::operator==(const Size& other) const
	{
		return width == other.width && height == other.height;
	}

	inline bool Size::operator!=(const Size& other) const
	{
		return !operator==(other);
	}

	inline Size operator*(const Size& lhs,const Size& rhs)
	{
		return Size(lhs.width * rhs.width, lhs.height * rhs.height);
	}

	inline Size operator/(const Size& lhs,const Size& rhs)
	{
		return Size(lhs.width / rhs.width, lhs.height / rhs.height);
	}
}
