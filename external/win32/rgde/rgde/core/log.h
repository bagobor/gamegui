#pragma once

#include <string>

namespace rgde
{
	namespace core
	{
		namespace log
		{
			namespace utils
			{
				void output_debug_string(const std::wstring& msg);
				void output_debug_string(const std::string& msg);
			}
		}
	}
}