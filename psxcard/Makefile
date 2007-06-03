SRCDIR = src
FW_BINDIR = firmware
BINS = $(FW_BINDIR)/psxcard-fw psxcopy

all: $(BINS)

clean:
	rm -f $(FW_BINDIR)/*
	rm -f psxcard-fw.eeprom
	rm -f $(BINS)
	rm -f $(OBJS)


###############
## PC Software
###############

psxcopy: $(SRCDIR)/psxcopy.c
	cc -o $@ $< -lusb


###############
## Firmware
###############

TUSB_DIR = ../tusb
OBJS = \
	$(FW_BINDIR)/psxcard-fw.rel \
	$(FW_BINDIR)/descriptors.rel \
	$(TUSB_DIR)/src/usb_driver.rel \
	$(TUSB_DIR)/src/uart_driver.rel \
	$(TUSB_DIR)/src/util.rel

CFLAGS = -I $(TUSB_DIR)/include
LDFLAGS = --xram-loc 0xF800 --xram-size 0x500

# Install a temporary copy of the firmware via the USB bootloader, for development purposes.
install-ram: $(FW_BINDIR)/psxcard-fw
	$(TUSB_DIR)/bootload/bootload $<

# Install a nonvolatile copy of the firmware in the device's EEPROM
install-eeprom: $(FW_BINDIR)/psxcard-fw.eeprom
	$(TUSB_DIR)/bootload/i2c-rom -z 16384 -p 0xFF -w $<

# Intel HEX -> Flat Binary
$(FW_BINDIR)/psxcard-fw: $(FW_BINDIR)/psxcard-fw.ihx
	objcopy -I ihex -O binary $< $@

# Flat binary -> EEPROM image
$(FW_BINDIR)/psxcard-fw.eeprom: $(SRCDIR)/boot-eeprom.desc $(FW_BINDIR)/psxcard-fw
	$(TUSB_DIR)/bootload/i2c-header.py $< $@

$(FW_BINDIR)/psxcard-fw.ihx: $(OBJS)
	sdcc -o $@ $(OBJS) $(LDFLAGS)

$(FW_BINDIR)/%.rel: $(SRCDIR)/%.c $(TUSB_DIR)/include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

%.rel: %.c $(TUSB_DIR)/include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

