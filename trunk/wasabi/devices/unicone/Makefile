BINS = \
	loader_tool \
	gc_test \
	PyUnicone/_libunicone.so \
	firmware.bin

COMMON_OBJS = \
	src/bit_file.o \
	src/progress.o \
	src/progress_console.o \
	src/unicone_device.o \
	src/sha1.o

LOADER_TOOL_OBJS = $(COMMON_OBJS) \
	src/loader_tool.o

GC_TEST_OBJS = $(COMMON_OBJS) \
	src/gc_test.o

PYUNICONE_OBJS = $(COMMON_OBJS) \
	src/progress_python.o \
	PyUnicone/libunicone_wrap.o

TUSB_DIR = ../../../tusb

FIRMWARE_OBJS = \
	firmware/main.rel \
	firmware/descript.rel \
	firmware/fpga.rel \
	firmware/i2c.rel \
	$(TUSB_DIR)/src/uart_driver.rel \
	$(TUSB_DIR)/src/usb_driver.rel \
	$(TUSB_DIR)/src/util.rel

CFLAGS    += -I include -I src -I /usr/include/python2.3
LDFLAGS   += -lusb
SWIGFLAGS += -python -Isrc -Iinclude

FW_CFLAGS = -I $(TUSB_DIR)/include -I include
FW_LDFLAGS = --xram-loc 0xF800 --xram-size 0x500
FW_CC = sdcc

all: $(BINS)

loader_tool: $(LOADER_TOOL_OBJS)
	$(CC) -o $@ $(LOADER_TOOL_OBJS) $(LDFLAGS)

gc_test: $(GC_TEST_OBJS)
	$(CC) -o $@ $(GC_TEST_OBJS) $(LDFLAGS)

PyUnicone/_libunicone.so: $(PYUNICONE_OBJS)
	$(CC) -o $@ $(PYUNICONE_OBJS) $(LDFLAGS) -shared

firmware/firmware.ihx: $(FIRMWARE_OBJS)
	$(FW_CC) -o $@ $(FIRMWARE_OBJS) $(FW_LDFLAGS)

firmware.bin: firmware/firmware.ihx
	objcopy -I ihex -O binary $< $@

%.rel: %.c include/*.h firmware/*.h
	$(FW_CC) -o $@ -c $< $(FW_CFLAGS)

%.o: %.c include/*.h src/*.h
	$(CC) -o $@ -c $< $(CFLAGS)

PyUnicone/libunicone_wrap.c: PyUnicone/libunicone.i include/*.h src/*.h
	swig $(SWIGFLAGS) $<

clean:
	rm -f $(BINS)
	rm -f src/*.o PyUnicone/libunicone_wrap.c
	rm -f firmware/*.ihx firmware/*.lnk firmware/*.lst firmware/*.map
	rm -f firmware/*.mem firmware/*.rel firmware/*.rst firmware/*.sym
	rm -f firmware/*.asm firmware/*.bin firmware/*.rel
