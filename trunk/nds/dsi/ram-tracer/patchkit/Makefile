PATCHES := \
	sample-patch.elf \
	nvram-dump.elf \
	nvram-write.elf \
	register-dump.elf \

# Common files for all patches
LDSCRIPT := patch.ld
COMMON_SRC := iohook.c

# We use headers from libnds
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif

LIBNDS := $(DEVKITPRO)/libnds
CC := arm-eabi-gcc
OBJDUMP := arm-eabi-objdump

CFLAGS := -g -Os -combine -fwhole-program -I$(LIBNDS)/include -Werror
LDFLAGS := -nostdlib -Wl,-T,$(LDSCRIPT)

all: $(PATCHES)

# Compile and link in one step. This improves size optimization,
# since all source files can be optimized as a single unit.

%.elf: %.c $(COMMON_SRC) $(LDSCRIPT) Makefile *.h
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $< $(COMMON_SRC)

clean:
	rm -f $(PATCHES)
