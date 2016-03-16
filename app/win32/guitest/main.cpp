#include "ui_application.h"

#ifdef _WINDOWS
	void RedirectIOToConsole();
#else
	void RedirectIOToConsole() {}
#endif


int main()
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//_CrtSetBreakAlloc(9799);

	int width = 1024;
	int height = 768;
	std::wstring res;// (params);
	
	if (!res.empty())
	{
		size_t delim = res.find_first_of(L" ");
		if (delim != std::wstring::npos)
		{
			width = _wtoi(res.substr(0, delim).c_str());
			height = _wtoi(res.substr(delim).c_str());
		}
	}

	RedirectIOToConsole();

	{
		ApplicationGUI sample(width, height, "GUI sample");
		sample.run();
	}

    return 0;
}