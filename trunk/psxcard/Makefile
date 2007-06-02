TUSB_DIR = ../tusb
OBJS = \
	psxcard-fw.rel \
	descriptors.rel \
	$(TUSB_DIR)/src/usb_driver.rel \
	$(TUSB_DIR)/src/uart_driver.rel \
	$(TUSB_DIR)/src/util.rel

BINS = psxcard-fw psxcopy

CFLAGS = -I $(TUSB_DIR)/include
LDFLAGS = --xram-loc 0xF800 --xram-size 0x500

all: $(BINS)

psxcopy: psxcopy.c
	cc -o psxcopy psxcopy.c -lusb

psxcard-fw: psxcard-fw.ihx
	objcopy -I ihex -O binary psxcard-fw.ihx psxcard-fw

install: psxcard-fw
	$(TUSB_DIR)/bootload/bootload psxcard-fw

psxcard-fw.ihx: $(OBJS)
	sdcc -o psxcard-fw.ihx $(OBJS) $(LDFLAGS)

%.rel: %.c $(TUSB_DIR)/include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rst *.sym *.asm
	rm -f $(BINS)
	rm -f $(OBJS)
