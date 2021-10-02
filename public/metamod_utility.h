/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * Metamod:Source
 * Copyright (C) 2004-2010 AlliedModders LLC and authors.
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

#include "metamod_sharedef.h"
#include <cstdio>
#include <concepts>
#include <string>

// Requires the terminal has gcc/clang>=10!
// or on Windows vc142 or above!
#include <filesystem>

#if defined _WIN32
#include <shellapi.h>
#endif

NAMESPACE_METAMOD_BEGIN

// only accepts char/wchar_t
// otherwise is not accepted
template<typename T> concept Char_c = std::same_as<T, char> || std::same_as<T, wchar_t>;
template<typename T> concept String_c = std::same_as<T, std::string> || std::same_as<T, std::wstring>;

NAMESPACE_METAMOD_DETAIL_BEGIN

#if defined _WIN32
static void GetPlatformError(char* buffer, size_t maxlength)
{
    DWORD dw = GetLastError();
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)buffer,
        static_cast<DWORD>(maxlength),
        NULL);
}
#endif

inline void* LoadLib(const char* path, char* buffer, size_t maxlength)
{
    void* lib;

#if defined _WIN32
    lib = (void*)LoadLibrary(path);

    if (lib == NULL)
    {
        GetPlatformError(buffer, maxlength);
        return NULL;
    }
#elif defined __linux__ 
    lib = dlopen(path, RTLD_NOW);

    if (lib == NULL)
    {
        mm_Format(buffer, maxlength, "%s", dlerror());
        return NULL;
    }
#endif

    return lib;
}

inline void UnloadLib(void* lib)
{
#if defined _WIN32
    FreeLibrary((HMODULE)lib);
#elif defined __linux__ 
    dlclose(lib);
#endif
}

inline void* GetLibAddress(void* lib, const char* name)
{
#if defined _WIN32
    return GetProcAddress((HMODULE)lib, name);
#elif defined __linux__ 
    return dlsym(lib, name);
#endif
}

inline void TrimLeft(String_c auto& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

inline void TrimRight(String_c auto& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

inline void TrimString(String_c auto& s) {
    TrimLeft(s);
    TrimRight(s);
}

// Warning: Currently only handles "//" comment!
inline void RemoveComment(String_c auto& s) {
    if (s.rfind("//") != std::string::npos) s.erase(s.begin() + s.rfind("//"), s.end());
}

inline std::string GetGameName()
{
    char buffer[128];

    buffer[0] = '\0';
    bool bHasDedicated = false;

#if defined _WIN32
    static char game[128];

    LPWSTR pCmdLine = GetCommandLineW();
    int argc;
    LPWSTR* wargv = CommandLineToArgvW(pCmdLine, &argc);
    for (int i = 0; i < argc; ++i)
    {
        if (wcscmp(wargv[i], L"-game") == 0)
        {
            if (++i >= argc)
                break;

            wcstombs(buffer, wargv[i], sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';
        }
        else if (wcscmp(wargv[i], L"-dedicated") == 0)
        {
            bHasDedicated = true;
        }
    }

    LocalFree(wargv);
#elif defined __linux__
    FILE* pFile = fopen("/proc/self/cmdline", "rb");
    if (pFile)
    {
        char* arg = NULL;
        size_t argsize = 0;
        bool bNextIsGame = false;

        while (getdelim(&arg, &argsize, 0, pFile) != -1)
        {
            if (bNextIsGame)
            {
                strncpy(buffer, arg, sizeof(buffer));
                buffer[sizeof(buffer) - 1] = '\0';
                bNextIsGame = false;
            }

            if (strcmp(arg, "-game") == 0)
            {
                bNextIsGame = true;
            }
            else if (strcmp(arg, "-dedicated") == 0)
            {
                bHasDedicated = true;
            }
        }

        free(arg);
        fclose(pFile);
    }
#else
#error unsupported platform
#endif

    if (buffer[0] == 0)
    {
        // HackHackHack - Different engines have different defaults if -game isn't specified
        // we only use this for game detection, and not even in all cases. Old behavior was to 
        // give back ".", which was only really accurate for Dark Messiah. We'll add a special 
        // case for Source2 / Dota as well, since it only supports gameinfo loading, which relies
        // on accuracy here more than VSP loading.
        if (bHasDedicated)
        {
            strncpy(buffer, "dota", sizeof(buffer));
        }
        else
        {
            strncpy(buffer, ".", sizeof(buffer));
        }
    }

    return std::string(buffer);
}

inline bool PathCmp(const std::string& p1, const std::string& p2) { return std::filesystem::equivalent(p1, p2); }

// This func is used for split valve keyvalue
inline void KeySplit(const char* str, char* buf1, size_t len1, char* buf2, size_t len2)
{
    size_t start;
    size_t len = strlen(str);

    for (start = 0; start < len; start++)
    {
        if (!isspace(str[start]))
            break;
    }

    size_t end;
    for (end = start; end < len; end++)
    {
        if (isspace(str[end]))
            break;
    }

    size_t i, c = 0;
    for (i = start; i < end; i++, c++)
    {
        if (c >= len1)
            break;
        buf1[c] = str[i];
    }
    buf1[c] = '\0';

    for (start = end; start < len; start++)
    {
        if (!isspace(str[start]))
            break;
    }

    for (c = 0; start < len; start++, c++)
    {
        if (c >= len2)
            break;
        buf2[c] = str[start];
    }
    buf2[c] = '\0';
}

inline bool GetFileOfAddress(void* pAddr, char* buffer, size_t maxlength)
{
#if defined _WIN32
    MEMORY_BASIC_INFORMATION mem;
    if (!VirtualQuery(pAddr, &mem, sizeof(mem)))
        return false;
    if (mem.AllocationBase == NULL)
        return false;
    HMODULE dll = (HMODULE)mem.AllocationBase;
    GetModuleFileName(dll, (LPTSTR)buffer, static_cast<DWORD>(maxlength));
#elif defined __linux__ 
    Dl_info info;
    if (!dladdr(pAddr, &info))
        return false;
    if (!info.dli_fbase || !info.dli_fname)
        return false;
    const char* dllpath = info.dli_fname;
    snprintf(buffer, maxlength, "%s", dllpath);
#endif
    return true;
}

struct DynLibInfo
{
    void* baseAddress;
    size_t memorySize;
};

static inline bool GetLibraryInfo(const void* libPtr, DynLibInfo& lib)
{
    uintptr_t baseAddr;

    if (libPtr == NULL)
    {
        return false;
    }

#ifdef _WIN32

#ifdef _M_X64
    const WORD PE_FILE_MACHINE = IMAGE_FILE_MACHINE_AMD64;
    const WORD PE_NT_OPTIONAL_HDR_MAGIC = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
#else
    const WORD PE_FILE_MACHINE = IMAGE_FILE_MACHINE_I386;
    const WORD PE_NT_OPTIONAL_HDR_MAGIC = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
#endif

    MEMORY_BASIC_INFORMATION info;
    IMAGE_DOS_HEADER* dos;
    IMAGE_NT_HEADERS* pe;
    IMAGE_FILE_HEADER* file;
    IMAGE_OPTIONAL_HEADER* opt;

    if (!VirtualQuery(libPtr, &info, sizeof(MEMORY_BASIC_INFORMATION)))
    {
        return false;
    }

    baseAddr = reinterpret_cast<uintptr_t>(info.AllocationBase);

    /* All this is for our insane sanity checks :o */
    dos = reinterpret_cast<IMAGE_DOS_HEADER*>(baseAddr);
    pe = reinterpret_cast<IMAGE_NT_HEADERS*>(baseAddr + dos->e_lfanew);
    file = &pe->FileHeader;
    opt = &pe->OptionalHeader;

    /* Check PE magic and signature */
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || pe->Signature != IMAGE_NT_SIGNATURE || opt->Magic != PE_NT_OPTIONAL_HDR_MAGIC)
    {
        return false;
    }

    /* Check architecture	*/
    if (file->Machine != PE_FILE_MACHINE)
    {
        return false;
    }

    /* For our purposes, this must be a dynamic library */
    if ((file->Characteristics & IMAGE_FILE_DLL) == 0)
    {
        return false;
    }

    /* Finally, we can do this */
    lib.memorySize = opt->SizeOfImage;

#elif defined __linux__

#ifdef __x86_64__
    typedef Elf64_Ehdr ElfHeader;
    typedef Elf64_Phdr ElfPHeader;
    const unsigned char ELF_CLASS = ELFCLASS64;
    const uint16_t ELF_MACHINE = EM_X86_64;
#else
    typedef Elf32_Ehdr ElfHeader;
    typedef Elf32_Phdr ElfPHeader;
    const unsigned char ELF_CLASS = ELFCLASS32;
    const uint16_t ELF_MACHINE = EM_386;
#endif

    Dl_info info;
    ElfHeader* file;
    ElfPHeader* phdr;
    uint16_t phdrCount;

    if (!dladdr(libPtr, &info))
    {
        return false;
    }

    if (!info.dli_fbase || !info.dli_fname)
    {
        return false;
    }

    /* This is for our insane sanity checks :o */
    baseAddr = reinterpret_cast<uintptr_t>(info.dli_fbase);
    file = reinterpret_cast<ElfHeader*>(baseAddr);

    /* Check ELF magic */
    if (memcmp(ELFMAG, file->e_ident, SELFMAG) != 0)
    {
        return false;
    }

    /* Check ELF version */
    if (file->e_ident[EI_VERSION] != EV_CURRENT)
    {
        return false;
    }

    /* Check ELF architecture	*/
    if (file->e_ident[EI_CLASS] != ELF_CLASS || file->e_machine != ELF_MACHINE || file->e_ident[EI_DATA] != ELFDATA2LSB)
    {
        return false;
    }

    /* For our purposes, this must be a dynamic library/shared object */
    if (file->e_type != ET_DYN)
    {
        return false;
    }

    phdrCount = file->e_phnum;
    phdr = reinterpret_cast<ElfPHeader*>(baseAddr + file->e_phoff);

    for (uint16_t i = 0; i < phdrCount; i++)
    {
        ElfPHeader& hdr = phdr[i];

        /* We only really care about the segment with executable code */
        if (hdr.p_type == PT_LOAD && hdr.p_flags == (PF_X | PF_R))
        {
            /* From glibc, elf/dl-load.c:
             * c->mapend = ((ph->p_vaddr + ph->p_filesz + GLRO(dl_pagesize) - 1)
             * & ~(GLRO(dl_pagesize) - 1));
             *
             * In glibc, the segment file size is aligned up to the nearest page size and
             * added to the virtual address of the segment. We just want the size here.
             */
            lib.memorySize = PAGE_ALIGN_UP(hdr.p_filesz);
            break;
        }
    }
#endif

    lib.baseAddress = reinterpret_cast<void*>(baseAddr);

    return true;
}

inline void* FindPattern(const void* libPtr, const char* pattern, size_t len)
{
    DynLibInfo lib;
    bool found;
    char* ptr, * end;

    memset(&lib, 0, sizeof(DynLibInfo));

    if (!GetLibraryInfo(libPtr, lib))
    {
        return NULL;
    }

    ptr = reinterpret_cast<char*>(lib.baseAddress);
    end = ptr + lib.memorySize - len;

    while (ptr < end)
    {
        found = true;
        for (register size_t i = 0; i < len; i++)
        {
            if (pattern[i] != '\x2A' && pattern[i] != ptr[i])
            {
                found = false;
                break;
            }
        }

        if (found)
            return ptr;

        ptr++;
    }

    return NULL;
}

inline size_t FormatArgs(char* buffer, size_t maxlength, const char* fmt, va_list params)
{
    size_t len = vsnprintf(buffer, maxlength, fmt, params);

    if (len >= maxlength)
    {
        len = maxlength - 1;
        buffer[len] = '\0';
    }

    return len;
}

inline size_t Format(char* buffer, size_t maxlength, const char* fmt, ...)
{
    size_t len;
    va_list ap;

    va_start(ap, fmt);
    len = FormatArgs(buffer, maxlength, fmt, ap);
    va_end(ap);

    return len;
}

inline size_t PathFormat(char* buffer, size_t maxlen, const char* fmt, ...)
{
    size_t len;
    va_list ap;

    va_start(ap, fmt);
    len = FormatArgs(buffer, maxlen, fmt, ap);
    va_end(ap);

    for (size_t i = 0; i < len; i++)
    {
        if (buffer[i] == ALT_SEP_CHAR)
            buffer[i] = PATH_SEP_CHAR;
    }

    return len;
}

inline bool ResolvePath(const char* path, char* buffer, size_t maxlength, bool bSource2)
{
    char tmp[PLATFORM_MAX_PATH];
    if (bSource2)
    {
        Format(tmp, sizeof(tmp), "../../%s", path);
        path = tmp;
    }
#if defined _WIN32
    return _fullpath(buffer, path, maxlength) != NULL;
#elif defined __linux__ 
    assert(maxlength >= PATH_MAX);
    return realpath(path, buffer) != NULL;
#endif
}

NAMESPACE_METAMOD_DETAIL_END

template<class...Args>
inline size_t Format(char* buffer, size_t maxlen, const char* fmt, Args...args) { return detail::Format(buffer, maxlen, fmt, args...); }

// Using LoadLib instead of LoadLibrary to avoid conflict of the macro of LoadLibrary......
inline void* LoadLib(const char* path, char* buffer, size_t maxlength) { return detail::LoadLib(path, buffer, maxlength); }

inline void* GetLibAddress(void* lib, const char* name) { return detail::GetLibAddress(lib, name); }

inline void UnloadLib(void* lib) { detail::UnloadLib(lib); }

inline bool ResolvePath(const char* path, char* buffer, size_t maxlength, bool source2) { return detail::ResolvePath(path, buffer, maxlength, source2); }

template<class...Args>
inline size_t PathFormat(char* buffer, size_t maxlen, const char* fmt, Args...args) { return detail::PathFormat(buffer, maxlen, fmt, args...); }

inline void TrimString(String_c auto& s) { detail::TrimString(s); }

inline void RemoveComment(String_c auto& s) { detail::RemoveComment(s); }

// TBD: Can change it to std::map?
inline void KeySplit(const char* str, char* buf1, size_t len1, char* buf2, size_t len2) { detail::KeySplit(str, buf1, len1, buf2, len2); }

inline bool PathCmp(const std::string& p1, const std::string& p2) { return detail::PathCmp(p1, p2); }

inline bool GetFileOfAddress(void* pAddr, char* buffer, size_t maxlength) { return detail::GetFileOfAddress(pAddr, buffer, maxlength); }

inline void* FindPattern(const void* libPtr, const char* pattern, size_t len) { return detail::FindPattern(libPtr, pattern, len); }

std::string GetGameName() { return detail::GetGameName(); }



NAMESPACE_METAMOD_END