// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef WINCE
#define POCKETPC2003_UI_MODEL
#define UNDER_CE
#define _UNICODE
#define UNICODE
//
//_DEBUG
//_WIN32_WCE=$(CEVER)
//
//$(PLATFORMDEFINES)
//WINCE
//DEBUG
//_WINDOWS
//$(ARCHFAM)
//$(_ARCHFAM_)
//_UNICODE
//UNICODE
//POCKETPC2003_UI_MODEL


	#pragma comment(linker, "/nodefaultlib:libc.lib")
	#pragma comment(linker, "/nodefaultlib:libcd.lib")

	// NOTE - this value is not strongly correlated to the Windows CE OS version being targeted
	#define WINVER _WIN32_WCE

	#include <ceconfig.h>
	#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	#define SHELL_AYGSHELL
	#endif

	#ifdef _CE_DCOM
	#define _ATL_APARTMENT_THREADED
	#endif

	#include <aygshell.h>
	#pragma comment(lib, "aygshell.lib") 
#else
	#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
	#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
	#endif
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <math.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <memory>

extern "C"
{
#include <lstate.h>
#include <lauxlib.h>
#include <lua.h>	
#include <lualib.h>
}

#include <luabind/luabind.hpp>
#include <luabind/iterator_policy.hpp>
#include <luabind/out_value_policy.hpp>


#undef max
#undef min

typedef unsigned int utf32;