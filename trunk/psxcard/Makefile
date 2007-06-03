BINS = psxcard-fw psxcopy

all: $(BINS)

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rst *.sym *.asm
	rm -f psxcard-fw.eeprom
	rm -f $(BINS)
	rm -f $(OBJS)


###############
## PC Software
###############

psxcopy: psxcopy.c
	cc -o psxcopy psxcopy.c -lusb


###############
## Firmware
###############

TUSB_DIR = ../tusb
OBJS = \
	psxcard-fw.rel \
	descriptors.rel \
	$(TUSB_DIR)/src/usb_driver.rel \
	$(TUSB_DIR)/src/uart_driver.rel \
	$(TUSB_DIR)/src/util.rel

CFLAGS = -I $(TUSB_DIR)/include
LDFLAGS = --xram-loc 0xF800 --xram-size 0x500

# Install a temporary copy of the firmware via the USB bootloader, for development purposes.
install-ram: psxcard-fw
	$(TUSB_DIR)/bootload/bootload psxcard-fw

# Install a nonvolatile copy of the firmware in the device's EEPROM
install-eeprom: psxcard-fw.eeprom
	$(TUSB_DIR)/bootload/i2c-rom -z 16384 -p 0xFF -w psxcard-fw.eeprom

# Intel HEX -> Flat Binary
psxcard-fw: psxcard-fw.ihx
	objcopy -I ihex -O binary psxcard-fw.ihx psxcard-fw

# Flat binary -> EEPROM image
psxcard-fw.eeprom: psxcard-fw boot-eeprom.desc
	$(TUSB_DIR)/bootload/i2c-header.py boot-eeprom.desc psxcard-fw.eeprom

psxcard-fw.ihx: $(OBJS)
	sdcc -o psxcard-fw.ihx $(OBJS) $(LDFLAGS)

%.rel: %.c $(TUSB_DIR)/include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

