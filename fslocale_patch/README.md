# Filesystem Locale Hook

A small Windows C DLL that redirects selected ANSI filesystem API calls to their Unicode Win32 equivalents. It targets legacy applications that pass paths in a non-Unicode code page: the wrappers convert non-ASCII path bytes with a configured code page, then call the wide-character API.

## Status

Classification: reference / deduplicate.

This is not a complete hook package. The API wrappers are here, but the hook installation layer is not: nothing yet redirects the intercepted calls or initializes the `real_*` function pointers. Treat this as a reference for the conversion logic, not a drop-in DLL.

## Supported wrappers

- `CreateFileA` -> `CreateFileW`
- `FindFirstFileA` -> `FindFirstFileW`
- `SetCurrentDirectoryA` -> `SetCurrentDirectoryW`

Printable-ASCII paths pass through unchanged. A path is converted only if it contains a byte outside `0x20`-`0x7E`.

## Configuration

Set `LOCALE_CP` to a Windows code page ID before the DLL loads. If it is unset, the code defaults to UTF-8 (`65001`).

```powershell
$env:LOCALE_CP = "932"
```

## Build

Windows-only; depends on `windows.h`. With MSYS2 MinGW32:

```powershell
C:\msys64\mingw32\bin\gcc.exe -shared -Wall -Wextra -o filesystem_locale_hook.dll filesystem_locale_hook.c
```

## How it works

1. `DllMain` runs on `DLL_PROCESS_ATTACH`.
2. `init_target_codepage` reads `LOCALE_CP`.
3. An external hook layer (not included) redirects the intercepted `A` calls to the `x_*` wrappers.
4. Each wrapper checks whether the path needs conversion.
5. If so, it converts the path and calls the Unicode Win32 API.
6. ASCII paths, and paths where conversion fails, fall back to the original ANSI API.

The `real_*` pointers are placeholders for whatever hook framework owns the original call targets; the missing installer is what would populate them. The exported no-op `dropkick` function is kept from the original artifact, where it may have been part of the loader convention that expected this DLL.
