#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define EXPORTABLE __declspec(dllexport)

EXPORTABLE void dropkick(void) {}

#define DEFAULT_CODEPAGE CP_UTF8
#define PATH_BUFFER_CHARS 1024

static UINT target_codepage = DEFAULT_CODEPAGE;

static void init_target_codepage(void) {
    const char* configured_codepage = getenv("LOCALE_CP");

    if (!configured_codepage || configured_codepage[0] == '\0') {
        target_codepage = DEFAULT_CODEPAGE;
        return;
    }

    target_codepage = (UINT)atoi(configured_codepage);
}

typedef HANDLE __stdcall tCreateFileA(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);
typedef HANDLE __stdcall tFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
typedef BOOL   __stdcall tSetCurrentDirectoryA(LPCSTR lpPathName);

static tSetCurrentDirectoryA* real_SetCurrentDirectoryA = NULL;
static tFindFirstFileA* real_FindFirstFileA = NULL;
static tCreateFileA* real_CreateFileA = NULL;

#define PRINTABLE_ASCII_MAX 0x7E
#define PRINTABLE_ASCII_MIN 0x20

static BOOL path_needs_codepage_conversion(const char* path) {
    size_t i = 0;

    for (i = 0; i < strlen(path); i++) {
        unsigned char character = (unsigned char)path[i];

        if (character > PRINTABLE_ASCII_MAX || character < PRINTABLE_ASCII_MIN) {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL __stdcall x_SetCurrentDirectoryA(LPCSTR lpPathName) {
    if (lpPathName && path_needs_codepage_conversion(lpPathName)) {
        wchar_t w_file_path[PATH_BUFFER_CHARS] = {0};

        if (MultiByteToWideChar(target_codepage, 0, lpPathName, -1, w_file_path, PATH_BUFFER_CHARS)) {
            return SetCurrentDirectoryW(w_file_path);
        }
    }

    if (!real_SetCurrentDirectoryA) {
        return SetCurrentDirectoryA(lpPathName);
    }

    return real_SetCurrentDirectoryA(lpPathName);
}

HANDLE __stdcall x_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
    if (lpFileName && lpFindFileData && path_needs_codepage_conversion(lpFileName)) {
        wchar_t w_file_path[PATH_BUFFER_CHARS] = {0};

        if (MultiByteToWideChar(target_codepage, 0, lpFileName, -1, w_file_path, PATH_BUFFER_CHARS)) {
            WIN32_FIND_DATAW find_file_data_w = {0};
            HANDLE hFile = FindFirstFileW(w_file_path, &find_file_data_w);

            if (hFile == INVALID_HANDLE_VALUE) {
                return hFile;
            }

            lpFindFileData->dwFileAttributes = find_file_data_w.dwFileAttributes;
            lpFindFileData->ftCreationTime   = find_file_data_w.ftCreationTime;
            lpFindFileData->ftLastAccessTime = find_file_data_w.ftLastAccessTime;
            lpFindFileData->ftLastWriteTime  = find_file_data_w.ftLastWriteTime;
            lpFindFileData->nFileSizeHigh    = find_file_data_w.nFileSizeHigh;
            lpFindFileData->nFileSizeLow     = find_file_data_w.nFileSizeLow;
            lpFindFileData->dwReserved0      = find_file_data_w.dwReserved0;
            lpFindFileData->dwReserved1      = find_file_data_w.dwReserved1;
            WideCharToMultiByte(target_codepage, 0, find_file_data_w.cFileName, -1,
                                lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName),
                                NULL, NULL);
            WideCharToMultiByte(target_codepage, 0, find_file_data_w.cAlternateFileName, -1,
                                lpFindFileData->cAlternateFileName, sizeof(lpFindFileData->cAlternateFileName),
                                NULL, NULL);
            return hFile;
        }
    }

    if (!real_FindFirstFileA) {
        return FindFirstFileA(lpFileName, lpFindFileData);
    }

    return real_FindFirstFileA(lpFileName, lpFindFileData);
}

HANDLE __stdcall x_CreateFileA(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile) {
    if (lpFileName && path_needs_codepage_conversion(lpFileName)) {
        wchar_t w_file_path[PATH_BUFFER_CHARS] = {0};

        if (MultiByteToWideChar(target_codepage, 0, lpFileName, -1, w_file_path, PATH_BUFFER_CHARS)) {
            return CreateFileW(w_file_path,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
        }
    }

    if (!real_CreateFileA) {
        return CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
    }

    return real_CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    (void)hinstDLL;
    (void)lpvReserved;

    if (fdwReason == DLL_PROCESS_ATTACH) {
        init_target_codepage();
        // Hook installation must set the real_* pointers before redirecting calls here.
    }

    return TRUE;
}
