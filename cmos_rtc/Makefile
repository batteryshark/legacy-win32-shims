CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -O2
BUILD_DIR ?= build

TEST_BIN := $(BUILD_DIR)/rtc_test
SOURCES := rtc_test.c cmos_rtc.c
HEADERS := cmos_rtc.h

.PHONY: all test clean win32 win64

all: $(TEST_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_BIN): $(SOURCES) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

win32: $(BUILD_DIR)
	i686-w64-mingw32-gcc $(CFLAGS) $(SOURCES) -o $(BUILD_DIR)/rtc_test_win32.exe

win64: $(BUILD_DIR)
	x86_64-w64-mingw32-gcc $(CFLAGS) $(SOURCES) -o $(BUILD_DIR)/rtc_test_win64.exe

clean:
	rm -rf $(BUILD_DIR)
