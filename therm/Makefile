BINS = therm-daemon therm-rx.bin

CFLAGS := -Iinclude

DAEMON_OBJS = \
	daemon/main.o \
	daemon/receiver.o

TUSB_DIR = receiver/tusb

FW_OBJS = \
	receiver/main.rel \
	receiver/leds.rel \
	receiver/receiver.rel \
	receiver/tc74.rel \
	receiver/descript.rel \
	$(TUSB_DIR)/src/usb_driver.rel \
	$(TUSB_DIR)/src/uart_driver.rel \
	$(TUSB_DIR)/src/util.rel \
	$(TUSB_DIR)/src/i2c_driver.rel

FW_CFLAGS = -I $(TUSB_DIR)/include -I include
FW_LDFLAGS = --xram-loc 0xF800 --xram-size 0x500

all: $(BINS)

therm-daemon: $(DAEMON_OBJS)
	$(CC) -o $@ $(DAEMON_OBJS) $(LDFLAGS)

therm-rx.bin: receiver/therm-rx.ihx
	objcopy -I ihex -O binary $< $@

receiver/therm-rx.ihx: $(FW_OBJS)
	sdcc -o $@ $(FW_OBJS) $(FW_LDFLAGS)

%.rel: %.c $(TUSB_DIR)/include/*.h include/*.h
	sdcc -o $@ -c $< $(FW_CFLAGS)

install-eeprom: boot-eeprom.bin
	$(TUSB_DIR)/bootload/i2c-rom $<

boot-eeprom.bin: boot-eeprom.desc
	$(TUSB_DIR)/bootload/i2c-header.py $< $@

clean:
	rm -f receiver/*.ihx receiver/*.lnk receiver/*.lst receiver/*.map receiver/*.mem
	rm -f receiver/*.rst receiver/*.sym receiver/*.asm receiver/*.bin
	rm -f daemon/*.o
	rm -f $(BINS) $(FW_OBJS)
