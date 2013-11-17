#pragma once

#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#include <windows.h>
#include <shlobj.h>

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <map>
#include <vector>

#include <boost/timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>