#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#include <assert.h>

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cstring>

#include <list>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

#include <memory>
#include <unordered_map>