// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
	#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
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


#include <luabind/luabind.hpp>
#include <luabind/iterator_policy.hpp>
#include <luabind/out_value_policy.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "./xml/xml.h"

#undef max
#undef min

typedef unsigned int utf32;