TARGET := aggro_winmm.dll
SOURCE := aggro_winmm.c

CC := gcc
CFLAGS ?= -Wall -Wextra
LDFLAGS ?= -shared -static -static-libgcc
LDLIBS ?= -lgcc

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) -m32 $(CFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@

clean:
	$(RM) $(TARGET)
