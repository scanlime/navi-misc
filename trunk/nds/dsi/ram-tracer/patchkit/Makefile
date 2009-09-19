PATCHES := sample-patch.elf
LDSCRIPT := patch.ld

CC := arm-eabi-gcc
OBJDUMP := arm-eabi-objdump

ASFLAGS := -c
LDFLAGS := -nostdlib -Wl,-T,$(LDSCRIPT)

all: $(PATCHES)

%.elf: %.o $(LDSCRIPT)
	$(CC) $(LDFLAGS) -o $@ $<
	$(OBJDUMP) -x $@

%.o: %.s
	$(CC) $(ASFLAGS) -o $@ $<

clean:
	rm -f *.o *.elf
