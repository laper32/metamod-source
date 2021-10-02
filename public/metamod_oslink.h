/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * Metamod:Source
 * Copyright (C) 2004-2009 AlliedModders LLC and authors.
 * All rights reserved.
 * ======================================================
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

// Using C style due to it is OS related.

#if defined __WIN32__ || defined _WIN32 || defined WIN32
#	define WIN32_LEAN_AND_MEAN
#	define OS_WIN32
#	if defined _MSC_VER && _MSC_VER >= 1400
#		undef ARRAYSIZE
#	else
#		define mkdir(a) _mkdir(a)
#	endif
#	include <windows.h>
#	include <io.h>
#	include <direct.h>
#	define		dlmount(x)		LoadLibrary(x)
#	define		dlsym(x, s)		GetProcAddress(x, s)
#	define		dlclose(x)		FreeLibrary(x)
    const char* dlerror()
    {
        static char buf[1024];
        DWORD num;

        num = GetLastError();

        if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            num,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buf,
            sizeof(buf),
            NULL)
            == 0)
        {
            _snprintf(buf, sizeof(buf), "unknown error %x", num);
        }

        return buf;
    }
#	define		abspath(x, s)	_fullpath(x, s, sizeof(x))
#	define	PATH_SEP_STR		"\\"
#	define PATH_SEP_CHAR		'\\'
#	define ALT_SEP_CHAR		'/'
#	define PATH_SIZE			MAX_PATH
#	define strcasecmp			stricmp
    inline bool IsPathSepChar(char c) { return (c == '/' || c == '\\'); }
#elif defined __linux__ 
#	define OS_LINUX
#	include <dlfcn.h>
#	include <unistd.h>
#	include <sys/types.h>
#	include <dirent.h>
    typedef		void* HINSTANCE;
#	define		dlmount(x)		dlopen(x,RTLD_NOW)
#	define		abspath(x, s)	realpath(s, x)
#	define	PATH_SEP_STR		"/"
#	define PATH_SEP_CHAR		'/'
#	define ALT_SEP_CHAR		'\\'
#	define PATH_SIZE			PATH_MAX
#	ifndef stricmp
#		define	stricmp			strcasecmp
#	endif
#	ifndef strnicmp
#		define strnicmp		strncasecmp
#	endif
    inline bool IsPathSepChar(char c) { return (c == '/'); }
    int GetLastError()
    {
        return errno;
    }
#endif

#if defined __linux__ 
#	include <errno.h>
    int GetLastError();
#endif


#if defined __WIN32__ || defined _WIN32 || defined WIN32
#	define SMM_API extern "C" __declspec(dllexport)
#elif defined __GNUC__
#	define SMM_API extern "C" __attribute__ ((visibility("default")))	
#endif

#if defined __WIN32__ || defined _WIN32 || defined WIN32
    using int64_t = __int64;
    using uint64_t = unsigned __int64;
    using int32_t = __int32;
    using uint32_t = unsigned __int32;
#elif defined __GNUC__
#	include <stdint.h>
#endif

#if !defined __linux__
#	define snprintf	_snprintf
#	if defined _MSC_VER && _MSC_VER < 1500
#		define vsnprintf	_vsnprintf
#	endif
#endif
