#pragma once

#include "align.h"

namespace gui
{
	std::wstring UTF8ToUTF16(const std::string& text);
	std::string UTF16ToUTF8(const std::wstring& wtext);

	class Size;
	Size StringToSize(const std::string& str);
	std::string SizeToString(const Size& val);

	class  vec2;
	typedef	vec2 point;
	point StringToPoint(const std::string& str);
	std::string PointToString(const point& val);

	bool StringToBool(const std::string& str);
	const std::string& BoolToString(bool val);

	class Rect;
	Rect StringToArea(const std::string& str);
	Rect StringToRect(const std::string& str);
	std::string RectToString(const Rect& val);

	Align StringToAlign(const std::string& str);
	std::string AlignToString(Align val);

	unsigned int StringToAlignment(const std::string& str);
	std::string AlignmentToString(unsigned int val);

	class Color;
	Color StringToColor(const std::string& str);
	std::string ColorToString(const Color& val);
	Color HexStringToColor(const std::string& str);
	std::string ColorToHexString(const Color& val);

}
