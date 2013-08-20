#include "resource.h"

#include <windows.h>
#include "guicon.h"
#include "xml2test.h"

using namespace gui;

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR params, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	int width = 1024;
	int height = 768;
	std::wstring res(params);

	if(!res.empty())
	{
		size_t delim = res.find_first_of(L" ");
		if(delim != std::wstring::npos)
		{
			width = _wtoi(res.substr(0, delim).c_str());
			height = _wtoi(res.substr(delim).c_str());
		}
	}

	RedirectIOToConsole();

	Xml2Test g_test(100, 100, width, height, L"GUI Test");
	g_test.run();
    
    return 0;
}