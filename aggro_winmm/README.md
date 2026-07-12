# aggro_winmm

Tiny Windows DLL shim that force-loads a local `winmm.dll` before later
DirectSound or CD audio wrapper code can resolve the system copy from
`System32`.

## Why it exists

Some older game and CDDA wrapper setups rely on a wrapper `winmm.dll` sitting
next to the game executable. A DirectSound initialization path can cause the
process to load the system `winmm.dll` first, which prevents the local wrapper
from being used. This shim is meant to be loaded early and bias resolution back
toward the local file.

## Build

Build a 32-bit DLL from an MSYS2 MinGW32 shell:

```sh
mingw32-make
```

From PowerShell, use the MSYS2 MinGW32 tools directly:

```powershell
C:\msys64\msys2_shell.cmd -mingw32 -defterm -no-start -here -c "cd /path/to/aggro_winmm && mingw32-make"
```

Deploy the built `aggro_winmm.dll` wherever your loader expects this early shim,
and keep the wrapper `winmm.dll` in the process working directory.

## Notes

- `dropkick` is an intentionally empty export so the DLL has a simple symbol for
  loaders or inspection tools to find.
- The shim does loader work from `DllMain` because it must run as early as
  possible. Treat this as a narrow compatibility hack, not a general-purpose DLL
  pattern.
- `aggro_winmm.dll` is generated build output. The runtime `winmm.dll` wrapper is
  supplied by the target setup and is not part of this source repo.
- `./winmm.dll` is intentionally resolved from the process working directory,
  not from the shim DLL's own directory.
