TUSB_DIR = tusb
OBJS = \
	therm-rx.rel \
	descript.rel \
	$(TUSB_DIR)/src/usb_driver.rel \
	$(TUSB_DIR)/src/uart_driver.rel \
	$(TUSB_DIR)/src/util.rel \
	$(TUSB_DIR)/src/i2c_driver.rel

BIN = therm-rx.bin

CFLAGS = -I $(TUSB_DIR)/include
LDFLAGS = --xram-loc 0xF800 --xram-size 0x500

all: $(BIN)

therm-rx.bin: therm-rx.ihx
	objcopy -I ihex -O binary $< $@

install: $(BIN)
	$(TUSB_DIR)/bootload/bootload $(BIN)

therm-rx.ihx: $(OBJS)
	sdcc -o therm-rx.ihx $(OBJS) $(LDFLAGS)

%.rel: %.c $(TUSB_DIR)/include/*.h
	sdcc -o $@ -c $< $(CFLAGS)

install-eeprom: boot-eeprom.bin
	$(TUSB_DIR)/bootload/i2c-rom $<

boot-eeprom.bin: boot-eeprom.desc
	$(TUSB_DIR)/bootload/i2c-header.py $< $@

clean:
	rm -f *.ihx *.lnk *.lst *.map *.mem *.rst *.sym *.asm *.bin
	rm -f $(BIN) $(OBJS)
