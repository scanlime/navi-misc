# Needs to point to the 'tusb' project in navi-misc
TUSB_DIR = ../../tusb

OBJS = main.rel descriptors.rel dac.rel

CFLAGS = -I $(TUSB_DIR)/include
LDFLAGS = --xram-loc 0xF800 --xram-size 0x500 -L $(TUSB_DIR)/src -ltusb

all: laser

laser: laser.ihx
	objcopy -I ihex -O binary laser.ihx laser

install: laser
	$(TUSB_DIR)/bootload/bootload laser

laser.ihx: $(OBJS)
	sdcc -o laser.ihx $(OBJS) $(LDFLAGS)

%.rel: %.c $(TUSB_DIR)/include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rel *.rst *.sym *.asm
	rm -f laser
