#include "stdafx.h"

//#include "window.h"
#include <guilib/guilib.h>
#include "../common/gui_filesystem.h"
extern "C"
{
#include <lstate.h>
#include <lauxlib.h>
#include <lua.h>	
#include <lualib.h>
}


using namespace std;

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 5000;

void RedirectIOToConsole()
{

	int hConHandle;

	//long lStdHandle;
	HANDLE lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	FILE *fp;

	// allocate a console for this app

	bool br = AllocConsole() == TRUE;
	assert( br );

	// set the screen buffer to be big enough to let us scroll text

	br = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo) == TRUE;
	assert( br );

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	br = SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize) == TRUE;
	assert( br );

	// redirect unbuffered STDOUT to the console	
	lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	assert( lStdHandle != INVALID_HANDLE_VALUE );
	assert( lStdHandle != NULL );

	hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
	assert( hConHandle != -1 );

	fp = _fdopen( hConHandle, "w" );
	assert(fp);

	*stdout = *fp;

	int ir = setvbuf( stdout, NULL, _IONBF, 0 );
	assert(	ir ==0 );

	// redirect unbuffered STDIN to the console

	lStdHandle = GetStdHandle(STD_INPUT_HANDLE);

	hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );

	*stdin = *fp;

	ir = setvbuf( stdin, NULL, _IONBF, 0 );
	assert(	ir ==0 );


	// redirect unbuffered STDERR to the console

	lStdHandle = GetStdHandle(STD_ERROR_HANDLE);

	hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stderr = *fp;

	ir = setvbuf( stderr, NULL, _IONBF, 0 );
	assert(	ir ==0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
	// point to console as well

	ios::sync_with_stdio( true );
}

INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR params, int )
{
	// Enable run-time memory check for debug builds.
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//#endif

	int width = 1024;
	int height = 768;
	//std::wstring res(params);

	//if(!res.empty())
	//{
	//	size_t delim = res.find_first_of(L" ");
	//	if(delim != std::wstring::npos)
	//	{
	//		width = _wtoi(res.substr(0, delim).c_str());
	//		height = _wtoi(res.substr(delim).c_str());
	//	}
	//}

	RedirectIOToConsole();

	if (lua_State* state = luaL_newstate()) {
		luaL_openlibs(state);


		lua_close(state);
	}

	//TestWindow test(100, 100, width, height, L"GUI Test");
	//test.run();

	return 0;
}