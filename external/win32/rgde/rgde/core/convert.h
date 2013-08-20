#pragma once

#include <string>

namespace rgde
{
	namespace core
	{
		namespace utils
		{
			std::string convert2str(const std::wstring& wstr);
			std::wstring convert2wstr(const std::string& str);
		}
	}
}