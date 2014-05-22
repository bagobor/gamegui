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

#include <luabind/luabind.hpp>
#include <luabind/raw_policy.hpp>


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

void print_hello(int number) {
	using namespace std;
	cout << "hello world " << number << endl;
}

class NumberPrinter {
public:
	NumberPrinter(int number) :
		m_number(number) 
	{
		int i = 5;
	}

	void print() {
		cout << m_number << endl;
	}

	luabind::object properties() { return m_properties; }

	luabind::object lua_indexmeta(luabind::object options, lua_State *L) {
		return options;
	}
	luabind::object lua_newindex(luabind::object options, lua_State *L) {
		return options;
	}

private:
	luabind::object m_properties;
	int m_number;
};

//INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR params, int )
int main()
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

	//RedirectIOToConsole();

	if (lua_State* state = luaL_newstate()) {
		luaL_openlibs(state);

		luabind::open(state);



		//#1 можно сохранить скриптовую таблицу объекта если сначала поставит объект в глобальную таблицу а потом прочитаь из нее
		// и сохранить в объекте
		//#2 разобраться с багом с эвентами, почему this разный. (так же не должно быть!)

		luabind::module(state)[
			luabind::def("print_hello", print_hello)
		];

		luabind::module(state)[
			luabind::class_<NumberPrinter>("NumberPrinter")
				.def(luabind::constructor<int>())				
				.def("__index", &NumberPrinter::lua_indexmeta, luabind::raw_policy<3>())
				.def("__newindex", &NumberPrinter::lua_newindex, luabind::raw_policy<3>())
		];

		luaL_dostring(
			state,
			"function add(first, second)\n"
			"  return first + second\n"
			"end\n"
			"print('result: ' .. add(1,2));"
			"local numbers = NumberPrinter(6)"
			"numbers.hi = 5;"
			);

		int res = luabind::call_function<int>(state, "add", 2, 3);

		lua_close(state);
	}

	//TestWindow test(100, 100, width, height, L"GUI Test");
	//test.run();

	return 0;
}