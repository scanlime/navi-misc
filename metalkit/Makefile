all: boot.img

CFLAGS := -fno-builtin -nostdinc -nostdlib -Os
LDFLAGS := -nostdlib -Wl,-N,-Ttext,7C00

OBJS := boot.o main.o vgatext.o pci.o

boot.img: boot.elf
	objcopy -O binary $< $@

boot.elf: $(OBJS)
	gcc $(LDFLAGS) -o $@ $(OBJS)

%.o: %.S
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) boot.img boot.elf
