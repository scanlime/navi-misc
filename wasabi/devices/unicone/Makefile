BINS = \
	loader_tool \
	led_test \
	gc_test \
	pyunicone/_libunicone.so \
	firmware.bin

COMMON_OBJS = \
	src/bit_file.o \
	src/progress.o \
	src/progress_console.o \
	src/unicone_device.o \
	src/sha1.o

LOADER_TOOL_OBJS = $(COMMON_OBJS) \
	src/loader_tool.o

LED_TEST_OBJS = $(COMMON_OBJS) \
	src/led_test.o

GC_TEST_OBJS = $(COMMON_OBJS) \
	src/gc_test.o

PYUNICONE_OBJS = $(COMMON_OBJS) \
	pyunicone/libunicone_wrap.o

FIRMWARE_OBJS = \
	firmware/main.rel \
	firmware/descript.rel \
	firmware/fpga.rel \
	firmware/i2c.rel

TUSB_DIR = ../../../tusb

CFLAGS += -I include -I src -I /usr/include/python2.3
LDFLAGS += -lusb

FW_CFLAGS = -I $(TUSB_DIR)/include -I include
FW_LDFLAGS = --xram-loc 0xF800 --xram-size 0x500 -L $(TUSB_DIR)/src -ltusb
FW_CC = sdcc

all: $(BINS)

loader_tool: $(LOADER_TOOL_OBJS)
	$(CC) -o $@ $(LOADER_TOOL_OBJS) $(LDFLAGS)

led_test: $(LED_TEST_OBJS)
	$(CC) -o $@ $(LED_TEST_OBJS) $(LDFLAGS) -lm

gc_test: $(GC_TEST_OBJS)
	$(CC) -o $@ $(GC_TEST_OBJS) $(LDFLAGS)

pyunicone/_libunicone.so: $(PYUNICONE_OBJS)
	$(CC) -o $@ $(PYUNICONE_OBJS) $(LDFLAGS) -shared

firmware/firmware.ihx: $(FIRMWARE_OBJS)
	$(FW_CC) -o $@ $(FIRMWARE_OBJS) $(FW_LDFLAGS)

firmware.bin: firmware/firmware.ihx
	objcopy -I ihex -O binary $< $@

%.rel: %.c include/*.h firmware/*.h
	$(FW_CC) -o $@ -c $< $(FW_CFLAGS)

%.o: %.c include/*.h src/*.h
	$(CC) -o $@ -c $< $(CFLAGS)

pyunicone/libunicone_wrap.c: pyunicone/libunicone.i include/*.h src/*.h
	swig -python -Isrc -Iinclude $<

clean:
	rm -f $(BINS) src/*.o pyunicone/libunicone_wrap.c
	rm -f firmware/*.ihx firmware/*.lnk firmware/*.lst firmware/*.map
	rm -f firmware/*.mem firmware/*.rel firmware/*.rst firmware/*.sym
	rm -f firmware/*.asm firmware/*.bin firmware/*.rel
