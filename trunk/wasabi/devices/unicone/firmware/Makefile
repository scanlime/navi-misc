TUSB_DIR = ../../../../tusb
OBJS = main.rel descript.rel fpga.rel i2c.rel

CFLAGS = -I $(TUSB_DIR)/include -I ../include
LDFLAGS = --xram-loc 0xF800 --xram-size 0x500 -L $(TUSB_DIR)/src -ltusb

all: unicone.bin

unicone.bin: unicone.ihx
	objcopy -I ihex -O binary unicone.ihx unicone.bin

install: unicone.bin
	$(TUSB_DIR)/bootload/bootload unicone.bin

unicone.ihx: $(OBJS)
	sdcc -o unicone.ihx $(OBJS) $(LDFLAGS)

%.rel: %.c
	sdcc -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm *.bin
