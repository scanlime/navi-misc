BINS = \
	loader_tool \
	led_test \
	gc_test \
	firmware.bin

COMMON_OBJS = \
	src/bit_file.o \
	src/progress.o \
	src/progress_console.o \
	src/device.o \
	src/sha1.o

LOADER_TOOL_OBJS = $(COMMON_OBJS) \
	src/loader_tool.o

LED_TEST_OBJS = $(COMMON_OBJS) \
	src/led_test.o \

GC_TEST_OBJS = $(COMMON_OBJS) \
	src/gc_test.o \

FIRMWARE_OBJS = \
	firmware/main.rel \
	firmware/descript.rel \
	firmware/fpga.rel \
	firmware/i2c.rel

TUSB_DIR = ../../../tusb

CFLAGS += -I include -g
LDFLAGS += -lusb

FW_CFLAGS = -I $(TUSB_DIR)/include -I include
FW_LDFLAGS = --xram-loc 0xF800 --xram-size 0x500 -L $(TUSB_DIR)/src -ltusb

all: $(BINS)

loader_tool: $(LOADER_TOOL_OBJS) src/*.h include/*.h
	$(CC) -o $@ $(LOADER_TOOL_OBJS) $(LDFLAGS)

led_test: $(LED_TEST_OBJS) src/*.h include/*.h
	$(CC) -o $@ $(LED_TEST_OBJS) $(LDFLAGS) -lm

gc_test: $(GC_TEST_OBJS) src/*.h include/*.h
	$(CC) -o $@ $(GC_TEST_OBJS) $(LDFLAGS)

firmware/firmware.ihx: $(FIRMWARE_OBJS)
	sdcc -o $@ $(FIRMWARE_OBJS) $(FW_LDFLAGS)

firmware.bin: firmware/firmware.ihx
	objcopy -I ihex -O binary $< $@

%.rel: %.c
	sdcc -o $@ -c $< $(FW_CFLAGS)

clean:
	rm -f $(BINS) src/*.o
	rm -f firmware/*.ihx firmware/*.lnk firmware/*.lst firmware/*.map
	rm -f firmware/*.mem firmware/*.rel firmware/*.rst firmware/*.sym
	rm -f firmware/*.asm firmware/*.bin firmware/*.rel
