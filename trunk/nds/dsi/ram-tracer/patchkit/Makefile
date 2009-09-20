PATCHES := sample-patch.elf

LDSCRIPT := patch.ld
COMMON_SRC := iohook.c

CC := arm-eabi-gcc
OBJDUMP := arm-eabi-objdump
CFLAGS := -g -Os -combine -fwhole-program
LDFLAGS := -nostdlib -Wl,-T,$(LDSCRIPT)

all: $(PATCHES)

# Compile and link in one step. This improves size optimization,
# since all source files can be optimized as a single unit.

%.elf: %.c $(COMMON_SRC) $(LDSCRIPT) Makefile *.h
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $< $(COMMON_SRC)
	$(OBJDUMP) -d $@

clean:
	rm -f $(PATCHES)
