#pragma once

namespace gui
{
	enum Align
	{
		None	= 0,
		Left	= 1 << 0,
		Top		= 1 << 1,
		Right	= 1 << 2,
		Bottom	= 1 << 3,
		HCenter = 1 << 4,
		VCenter = 1 << 5
	};
	typedef std::vector<Align> Alignment;
	typedef unsigned int AlignmentBits;
}