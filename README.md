# legacy-win32-shims

Three small, self-contained Win32 compatibility shims for coaxing legacy
applications into running on modern Windows. Each is a single-purpose C shim
that fakes or redirects one piece of old platform behavior an application
expects. They are grouped here because each is too small to warrant its own
repository, but each is complete and documented on its own terms.

| Shim | What it does |
|---|---|
| [`aggro_winmm`](aggro_winmm/) | Force-loads a local `winmm.dll` before a DirectSound/CDDA path can bind the system copy, so a wrapper `winmm.dll` next to the executable wins. |
| [`cmos_rtc`](cmos_rtc/) | Emulates reading the CMOS real-time clock through the classic port `0x70`/`0x71` register-select flow, sourcing the fields from the host system time. |
| [`fslocale_patch`](fslocale_patch/) | Redirects selected ANSI filesystem calls (`CreateFileA`, `FindFirstFileA`, `SetCurrentDirectoryA`) to their Unicode equivalents for apps that pass paths in a non-Unicode code page. |

Each shim has its own `README.md` with the full description, build steps, and
status. `cmos_rtc` builds and runs its self-test on any platform with a C
compiler; `aggro_winmm` and `fslocale_patch` are Windows DLLs built with MinGW.

## Status

- `aggro_winmm` — complete; a narrow DLL-load-order hack done from `DllMain`.
- `cmos_rtc` — complete, with a self-test that validates the decoded timestamp.
- `fslocale_patch` — reference only. The ANSI→Unicode conversion wrappers are
  here, but the hook-installation layer that redirects the intercepted calls is
  not included; treat it as a reference for the conversion logic, not a
  drop-in DLL. See its README.

## Building

Each shim builds independently from its own directory. For the Windows DLLs you
need MinGW-w64 (`i686-w64-mingw32-gcc`); `cmos_rtc` builds natively with `make`.
