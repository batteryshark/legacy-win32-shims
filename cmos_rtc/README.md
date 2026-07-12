# cmos_rtc

`cmos_rtc` is a tiny C compatibility shim for code that expects to read a
CMOS real-time clock through the classic PC I/O-port flow:

1. Write a register number to port `0x70`.
2. Read the selected register value from port `0x71`.

Modern user-mode programs usually cannot perform that port I/O directly. This
project keeps the same register-select/read mental model while sourcing the RTC
fields from the host system time.

## What It Emulates

- BCD-encoded seconds, minutes, hours, day of week, day of month, month, year,
  and century registers.
- A small set of status registers commonly used by simple RTC readers.
- The register selection behavior of CMOS port `0x70` and data port `0x71`.

This is intentionally not a full motherboard CMOS implementation. Alarm
registers and most platform-specific CMOS bytes remain zero.

## API

```c
#include "cmos_rtc.h"

rtc_init();

rtc_set_register(RTC_SECONDS);
unsigned char seconds_bcd = rtc_get_register_value();

rtc_shutdown();
```

`rtc_get_register_value()` refreshes the time registers before returning the
selected byte, so the example does not need a background thread or privileged
I/O access.

## Build And Run

```sh
make test
```

The test program prints a decoded RTC timestamp and exits non-zero if any
register contains invalid BCD or an out-of-range time field.

The historical `compile.sh` entrypoint is still present and now delegates to
`make all`:

```sh
./compile.sh
```

Optional MinGW cross-build targets are available when those compilers are
installed:

```sh
make win32
make win64
```

## Project Layout

- `cmos_rtc.h` declares RTC register constants and the public shim API.
- `cmos_rtc.c` maintains the faux CMOS register buffer.
- `rtc_test.c` is a small executable smoke test and usage example.
- `Makefile` builds and runs the example.
